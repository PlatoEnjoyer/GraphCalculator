#include <grid_drawer.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

// Стили для отрисовки
static const float GRID_COLOR[3] = {0.9f, 0.9f, 0.9f};
static const float AXES_COLOR[3] = {0.0f, 0.0f, 0.0f};
static const float GRID_LINE_WIDTH = 1.0f;
static const float AXES_LINE_WIDTH = 2.0f;


void draw_letter_X(float x, float y, float size, float thickness) {
    glLineWidth(thickness);
    glBegin(GL_LINES);
    glVertex2f(x - size/2, y - size/2);
    glVertex2f(x + size/2, y + size/2);
    glVertex2f(x + size/2, y - size/2);
    glVertex2f(x - size/2, y + size/2);
    glEnd();
}

void draw_letter_Y(float x, float y, float size, float thickness) {
    glLineWidth(thickness);
    glBegin(GL_LINES);
    glVertex2f(x - size/3, y + size/2);
    glVertex2f(x, y + size/4);
    glVertex2f(x + size/3, y + size/2);
    glVertex2f(x, y + size/4);
    glVertex2f(x, y + size/4);
    glVertex2f(x, y - size/2);
    glEnd();
}

void draw_grid(float step, float width, float height) {
    if(step <= 0 || width <= 0 || height <= 0) return;
    
    // Сохраняем текущие настройки
    float prev_color[4];
    glGetFloatv(GL_CURRENT_COLOR, prev_color);
    float prev_line_width;
    glGetFloatv(GL_LINE_WIDTH, &prev_line_width);
    
    // Устанавливаем стиль сетки
    glColor3fv(GRID_COLOR);
    glLineWidth(GRID_LINE_WIDTH);
    
    glBegin(GL_LINES);
    // Вертикальные линии
    for(float x = -width; x <= width; x += step) {
        if(fabsf(x) > 0.001f) {
            glVertex2f(x, -height);
            glVertex2f(x, height);
        }
    }
    // Горизонтальные линии
    for(float y = -height; y <= height; y += step) {
        if(fabsf(y) > 0.001f) {
            glVertex2f(-width, y);
            glVertex2f(width, y);
        }
    }
    glEnd();
    
    // Восстанавливаем настройки
    glColor3fv(prev_color);
    glLineWidth(prev_line_width);
}

void draw_axes(float width, float height) {
    // Сохраняем настройки
    float prev_color[4];
    glGetFloatv(GL_CURRENT_COLOR, prev_color);
    float prev_line_width;
    glGetFloatv(GL_LINE_WIDTH, &prev_line_width);
    
    // Устанавливаем стиль осей
    glColor3fv(AXES_COLOR);
    glLineWidth(AXES_LINE_WIDTH);
    
    // Рисуем оси
    glBegin(GL_LINES);
    // Ось X
    glVertex2f(-width, 0.0f);
    glVertex2f(width, 0.0f);
    // Ось Y
    glVertex2f(0.0f, -height);
    glVertex2f(0.0f, height);
    glEnd();

    // Рисуем засечки для чётных чисел
    glBegin(GL_LINES);
    for (int i = -static_cast<int>(width); i <= width; i += 2) {
        if (i != 0) { // Пропускаем центр
            // Засечки на оси X
            glVertex2f(i, -0.1f);
            glVertex2f(i, 0.1f);
            
            // Засечки на оси Y
            glVertex2f(-0.1f, i);
            glVertex2f(0.1f, i);
        }
    }
    glEnd();
    
    // Рисуем стрелки для осей
    const float arrow_size = 0.2f;
    glBegin(GL_TRIANGLES);
    // Стрелка оси X
    glVertex2f(width, 0.0f);
    glVertex2f(width - arrow_size, -arrow_size);
    glVertex2f(width - arrow_size, arrow_size);
    
    // Стрелка оси Y
    glVertex2f(0.0f, height);
    glVertex2f(-arrow_size, height - arrow_size);
    glVertex2f(arrow_size, height - arrow_size);
    glEnd();

    // Восстанавливаем настройки
    glColor3fv(prev_color);
    glLineWidth(prev_line_width);
}