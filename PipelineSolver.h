#pragma once  // Защита от повторных включений

#include <vector>
#include <array>
using namespace std;

class PipelineSolver {
public:
    PipelineSolver(array<double, 3> userStartPoint = {NAN, NAN, NAN});  // Конструктор

    vector <array<double, 3>> PipelineSolver::getPipelineTrajectory();

private:
    array<double, 3> start_point;  // Переменная для первой точки
    vector<array<double, 3>> pipeline_dots;  // Массив точек
    vector<array<double, 6>> lines_coords;  // Массив координат линий
    vector <array <double, 3>> relativeCoordinates; //Массив координат относительного смещения точек

    void getLinesCoords();  // Метод для получения координат всех линий
    void findPipeline();  // Рекурсивная функция для нахождения решения
    void getRelativePointCoordinates(); //функция определения относительного смещения точек
};

