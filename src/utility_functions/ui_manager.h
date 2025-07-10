#pragma once
#include <string>
#include <vector>

void preparing();
void field_drawing(std::string& current_expr, bool& expr_needs_update, char function_input[256], std::string& error_message, int i);
void mouse_wheel(float& zoom);