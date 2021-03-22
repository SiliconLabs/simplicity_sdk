from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from pyradioconfig.parts.unit_test_part.phys.Phys_Essence import PHYS_Essence
from py_2_and_3_compatibility import *

class PHYS_virtual(IPhy):

    # Example renamed PHY from another PHY
    def PHY_Essence_868M_38p4kbps_renamed(self, model):

        phy = PHYS_Essence().PHY_Essence_868M_38p4kbps(model)

        return phy
