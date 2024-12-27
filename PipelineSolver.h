#pragma once  // Защита от повторных включений

#include <vector>
#include <array>
using namespace std;

class PipelineSolver {
public:
    PipelineSolver(bool useAutomaticStart = true, 
        array<double, 3> userStartPoint = {NAN, NAN, NAN});  // Конструктор

    void getPipelineSolution();  // Метод для получения решения
    vector <array<double, 3>> PipelineSolver::getPipelineTrajectory();

private:
    array<double, 3> start_point;  // Переменная для первой точки
    vector<array<double, 3>> pipiline_dots;  // Массив точек
    vector<array<double, 6>> lines_coords;  // Массив координат линий

    array<double, 3> defineFirstPoint();  // Метод для получения первой точки
    void getLinesCoords();  // Метод для получения координат всех линий
    void findPipeline();  // Рекурсивная функция для нахождения решения
    void printPipelineDots();  // Метод для вывода точек
};

