from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from pyradioconfig.parts.unit_test_part.phys.Phys_secret1 import PHYS_Secret1
from py_2_and_3_compatibility import *

class PHYS_virtual(IPhy):

    # Example renamed PHY from another PHY
    def PHY_Secret1_868M_38p4kbps_renamed(self, model):

        phy = PHYS_Secret1().PHY_Secret1_868M_38p4kbps(model)

        return phy
