# -*- coding: utf-8 -*-

import pythoncom
from win32com.client import Dispatch, gencache

class KompasAPI:
    '''
    Класс для подключения к КОМПАС-3D.
    При super() наследовании передает основные интефейсы:
    module - API компаса, app - экземпляр Application,
    const - константы Компаса
    '''
    def __init__(self):
        self.module = gencache.EnsureModule("{69AC2981-37C0-4379-84FD-5DD2F3C0A520}", 0, 1, 0)
        self.app = self.module.IApplication(Dispatch("Kompas.Application.7")._oleobj_.QueryInterface(self.module.IApplication.CLSID, 
                                                                                                     pythoncom.IID_IDispatch))
        self.const = gencache.EnsureModule("{2CAF168C-7961-4B90-9DA2-701419BEEFE3}", 0, 1, 0).constants
        self.const2D = gencache.EnsureModule("{75C9F5D0-B5B8-4526-8681-9903C567D2ED}", 0, 1, 0).constants
        
        self.module5 = gencache.EnsureModule("{0422828C-F174-495E-AC5D-D31014DBBE87}", 0, 1, 0)
        self.app5 = self.module5.KompasObject(Dispatch("Kompas.Application.5")._oleobj_.QueryInterface(self.module5.KompasObject.CLSID, pythoncom.IID_IDispatch))
