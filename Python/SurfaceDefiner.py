# -*- coding: utf-8 -*-

import pythoncom
from win32com.client import Dispatch, gencache

from kAPI import KompasAPI

class SurfaceDefiner(KompasAPI):
    def __init__(self):
        super().__init__()
        self._getSelectedFace()


    def getSelectedFace(self):
        doc = self.app.ActiveDocument
        doc3D = self.module.IKompasDocument3D(doc)
        iSelectMng = doc3D.SelectionManager
        selected_objects = iSelectMng.SelectedObjects
        if selected_objects:
            iMathCurve = selected_objects.MathCurve
            center = iMathCurve.GetCentre()
            for coord in center:
                if coord is True:
                    pass
                print(round(coord, 3))
                print(coord)

    def _getSelectedFace(self):
        doc3D = self.app5.ActiveDocument3D()
        iSelectMng = doc3D.GetSelectionMng()
        print(iSelectMng.First().GetDefinition())




if __name__=="__main__":
    SurfaceDefiner()