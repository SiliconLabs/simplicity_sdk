from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from py_2_and_3_compatibility import *

class PHYS_virtual_studio(IPhy):

    # Example renamed PHY from another PHY
    def PHY_Essence_868M_38p4kbps_renamed_virtual_studio(self, model):

        phy = self._makePhy(model, model.profiles.Base, 'PHY Essence 868M 38p4kbps virtual studio PHY')

        #modem_model.vars.shaping_filter.value_forced = model.vars.shaping_filter.var_enum.NONE
        phy.profile_inputs.shaping_filter.values = model.vars.shaping_filter.var_enum.NONE

        phy.profile_inputs.base_frequency_hz.values = long(868300000)

        return phy
