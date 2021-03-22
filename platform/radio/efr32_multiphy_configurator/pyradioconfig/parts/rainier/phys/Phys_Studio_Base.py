from pyradioconfig.parts.bobcat.phys.Phys_Studio_Base import PHYS_Studio_Base_Bobcat
from pyradioconfig.parts.common.phys.phy_common import PHY_COMMON_FRAME_INTERNAL


class PhysStudioBaseRainier(PHYS_Studio_Base_Bobcat):

    def Studio_2GFSK_base(self, phy, model):

        super().Studio_2GFSK_base(phy, model)
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
