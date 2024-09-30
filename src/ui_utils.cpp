#include "ui_utils.h"

ImVec2 operator+(ImVec2 a, ImVec2 b){
    return ImVec2(a.x + b.x, a.y + b.y);
}
ImVec2 operator-(ImVec2 a, ImVec2 b){
    return ImVec2(a.x - b.x, a.y - b.y);
}