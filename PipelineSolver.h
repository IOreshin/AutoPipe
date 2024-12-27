#pragma once  // ������ �� ��������� ���������

#include <vector>
#include <array>
using namespace std;

class PipelineSolver {
public:
    PipelineSolver(bool useAutomaticStart = true, 
        array<double, 3> userStartPoint = {NAN, NAN, NAN});  // �����������

    void getPipelineSolution();  // ����� ��� ��������� �������
    vector <array<double, 3>> PipelineSolver::getPipelineTrajectory();

private:
    array<double, 3> start_point;  // ���������� ��� ������ �����
    vector<array<double, 3>> pipiline_dots;  // ������ �����
    vector<array<double, 6>> lines_coords;  // ������ ��������� �����

    array<double, 3> defineFirstPoint();  // ����� ��� ��������� ������ �����
    void getLinesCoords();  // ����� ��� ��������� ��������� ���� �����
    void findPipeline();  // ����������� ������� ��� ���������� �������
    void printPipelineDots();  // ����� ��� ������ �����
};

