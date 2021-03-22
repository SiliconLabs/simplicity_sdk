from pyradioconfig.parts.bobcat.phys.Phys_Studio_Connect import PHYS_connect_Bobcat
from py_2_and_3_compatibility import *

class PhysStudioConnectRainier(PHYS_connect_Bobcat):

    def Connect_base(self, phy, model):

        super().Connect_base(phy, model)
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
