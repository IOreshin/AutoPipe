# -*- coding: utf-8 -*-

import pythoncom
from win32com.client import Dispatch, gencache

from kAPI import KompasAPI

class PipelineSolver(KompasAPI):
    def __init__(self):
        super().__init__()
        #self.firts_point = self.define_first_point()
        #self.pipeline_dots = [self.firts_point]
        self.lines_coords = self.get_lines_coords()
        print(self.lines_coords)
        #self.find_pipeline()

        #print(self.pipeline_dots)

    def define_first_point(self):
        doc = self.app.ActiveDocument
        doc3D = self.module.IKompasDocument3D(doc)
        iSelectMng = doc3D.SelectionManager
        iVertex = iSelectMng.SelectedObjects
        first_point = list(iVertex.GetPoint())
        return first_point[1:]


    def get_lines_coords(self):
        doc3D = self.app5.ActiveDocument3D()
        part = doc3D.GetPart(self.const.pTop_Part)
        iEntityCollection = part.EntityCollection(self.const.o3d_edge)
        if (iEntityCollection):
            entity_count = iEntityCollection.GetCount()
            lines_coords = []
            for i in range(entity_count):
                iEntity = iEntityCollection.GetByIndex(i)
                iEntityDefinition = iEntity.GetDefinition()
                if iEntityDefinition.IsStraight() is True:
                    curve3D = iEntityDefinition.GetCurve3D()
                    iLineSeg = curve3D.GetCurveParam()
                    print(iLineSeg.GetFirstPoint())
                    


            return lines_coords
        
    def find_pipeline(self):
        if self.lines_coords:
            for pair_coords in self.lines_coords:
                first_point = pair_coords[:3]
                second_point = pair_coords[3:]
                if second_point == self.firts_point:
                    self.pipeline_dots.append(first_point)
                    self.lines_coords.remove(pair_coords)
                    self.firts_point = first_point
                    self.find_pipeline()
                elif first_point == self.firts_point:
                    self.pipeline_dots.append(second_point)
                    self.lines_coords.remove(pair_coords)
                    self.firts_point = second_point
                    self.find_pipeline()

                

PipelineSolver()
