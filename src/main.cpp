#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <muParser.h>
#include <vector>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <grid_drawer.h>
#include <ui_manager.h>


struct Point {
    float x, y;
};

int main() {
    // Инициализация muParser
    mu::Parser parser;
    double x = 0.0;
    parser.DefineVar("x", &x);

    float zoom = 1.0f; // Глобальная переменная для масштаба

    char function_input[256] = "";
    std::string current_expr = function_input;
    bool expr_needs_update = true;

    const ImVec4 GRAPH_COLORS[] = {
    ImVec4(0.00f, 0.55f, 0.80f, 1.0f),  // Cиний
    ImVec4(0.90f, 0.30f, 0.20f, 1.0f),  // Красный
    ImVec4(0.00f, 0.65f, 0.30f, 1.0f),  // Зеленый
    ImVec4(0.75f, 0.35f, 0.85f, 1.0f),  // Фиолетовый
    ImVec4(0.95f, 0.65f, 0.10f, 1.0f)   // Оранжевый
    };

    // Для отображения ошибок
    std::string error_message;

    // Структура графика
    struct Graph
    {
        std::vector<Point> points;
        std::string error_message;
        bool expr_needs_update = true;
        std::string current_expr;
        char function_input[256] = "";
        bool visible = true;
        Graph() {};
        Graph(std::vector<Point> pnts, const std::string& expr, std::string error): 
        points(pnts), error_message(error), current_expr(expr) {};
    };
    
    std::vector<Graph> graphs;
    std::vector<Point> points;
    graphs.push_back(Graph(points, error_message, current_expr));

    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Ошибка инициализации GLFW\n";
        return -1;
    }

    // Получаем размеры экрана
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(1600, 900, "Graphs calculator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Не удалось создать окно GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Ошибка инициализации GLEW\n";
        return -1;
    }

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Настройка рендерера и бэкенда GLFW
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Получаем данные о размерах буффера окна
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        // Подготовка ImGui
        preparing();

        // Отрисовка окна для ввода
        ImGui::Begin("Graph calc");

        // Изменение мастшаба
        mouse_wheel(zoom);
        float visibleWidth = 20.0f / zoom;
        float visibleHeight = visibleWidth * (height / (float)width);

        // Попытка сделать динамические границы для отрисовки
        double x_start = -visibleWidth / 2;
        double x_end = visibleWidth / 2;
        double x_step = (x_end - x_start) / 1000;

        for (int i = 0; i < graphs.size(); i++) {
            ImGui::PushID(i); // Добавляем id i-го графика
            // Чекбокс видимости графика
            ImGui::Checkbox("##Visible", &graphs[i].visible);
            ImGui::SameLine();
            field_drawing(graphs[i].current_expr, graphs[i].expr_needs_update, graphs[i].function_input, graphs[i].error_message, i);
            if (graphs[i].expr_needs_update)
            {
                graphs[i].points.clear();
                graphs[i].error_message.clear();

                try {
                    parser.SetExpr(graphs[i].current_expr);

                    for (x = -100.0f; x <= 100.0f; x += 0.01f)
                    {
                        double result = parser.Eval();
                        graphs[i].points.push_back({(float)x, (float)result});
                    }
                }
                catch (mu::Parser::exception_type& e) {
                    graphs[i].error_message = "Error: " + std::string(e.GetMsg());
                }

                graphs[i].expr_needs_update = false;
            }
            ImGui::PopID();
        }
        if (ImGui::Button("+")) {
            graphs.push_back(Graph());
        }
        ImGui::SameLine();
        ImGui::Text("Add new field");
        ImGui::End();
        ImGui::Render(); // Готовим данные для рендеринга

        // Настройка области для вывода графики на экран
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(
            -visibleWidth / 2, visibleWidth / 2,
            -visibleHeight / 2, visibleHeight / 2,
            -1, 1
        );


        // Очистка экрана
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Рисуем координатную сетку и оси координат
        draw_grid(1.0f, visibleWidth/2, visibleHeight/2);
        draw_axes(visibleWidth/2, visibleHeight/2);

        // Отрисовка графиков
        for (int i = 0; i < graphs.size(); i++) {
            if (graphs[i].error_message.empty() && graphs[i].visible)
            {
                const ImVec4& line_color = GRAPH_COLORS[i];
                glColor3f(line_color.x, line_color.y, line_color.z);
                glLineWidth(1.7f);
                glEnable(GL_LINE_SMOOTH);
                glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
                glBegin(GL_LINE_STRIP);
                for (const auto& p : graphs[i].points) {
                    glVertex2f(p.x, p.y);
                }
                glEnd();
                glLineWidth(1.0f);
            }
        }

        // Рендеринг ImGui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Меняем буферы
        glfwSwapBuffers(window);
    }
    

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
