'''AUTOGENERATED BY test_gen_wisun_FAN1v1_py_files() - do not modify!'''

from pyradioconfig.calculator_model_framework.decorators.phy_decorators import do_not_inherit_phys
from pyradioconfig.parts.ocelot.phys.phys_studio_wisun_fan_1_1 import PhysStudioWisunFan1v1Jumbo, WisunFan1v1ChannelParamsJumbo

@do_not_inherit_phys
class PhysStudioWisunFan1v1VirtualJumbo(PhysStudioWisunFan1v1Jumbo):

    def PHY_WISUN_FAN_1v1_915MHz_Plan48_2FSK_1b_AZ_NZ(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('AZ_NZ', 0x02, 48, 200000, 915200000, 'Wi-SUN FAN, AZ_NZ 915-928 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_915MHz_Plan48_2FSK_2a_AZ_NZ(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('AZ_NZ', 0x03, 48, 200000, 915200000, 'Wi-SUN FAN, AZ_NZ 915-928 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_915MHz_Plan49_2FSK_3_AZ_NZ(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('AZ_NZ', 0x05, 49, 400000, 915400000, 'Wi-SUN FAN, AZ_NZ 915-928 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_915MHz_Plan49_2FSK_4a_AZ_NZ(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('AZ_NZ', 0x06, 49, 400000, 915400000, 'Wi-SUN FAN, AZ_NZ 915-928 MHz, Mode4a (2FSK 200kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_915MHz_Plan49_2FSK_5_AZ_NZ(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('AZ_NZ', 0x08, 49, 400000, 915400000, 'Wi-SUN FAN, AZ_NZ 915-928 MHz, Mode5 (2FSK 300kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan128_2FSK_1b_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x02, 128, 250000, 920625000, 'Wi-SUN FAN, CN 920-924 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan128_2FSK_2a_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x03, 128, 250000, 920625000, 'Wi-SUN FAN, CN 920-924 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan128_2FSK_3_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x05, 128, 250000, 920625000, 'Wi-SUN FAN, CN 920-924 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_779MHz_Plan144_2FSK_1b_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x02, 144, 200000, 779200000, 'Wi-SUN FAN, CN 779-787 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_779MHz_Plan144_2FSK_2a_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x03, 144, 200000, 779200000, 'Wi-SUN FAN, CN 779-787 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_779MHz_Plan145_2FSK_3_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x05, 145, 400000, 779400000, 'Wi-SUN FAN, CN 779-787 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_779MHz_Plan145_2FSK_4a_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x06, 145, 400000, 779400000, 'Wi-SUN FAN, CN 779-787 MHz, Mode4a (2FSK 200kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_779MHz_Plan145_2FSK_5_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x08, 145, 400000, 779400000, 'Wi-SUN FAN, CN 779-787 MHz, Mode5 (2FSK 300kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_470MHz_Plan160_2FSK_1b_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x02, 160, 200000, 470200000, 'Wi-SUN FAN, CN 470-510 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_470MHz_Plan160_2FSK_2a_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x03, 160, 200000, 470200000, 'Wi-SUN FAN, CN 470-510 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_470MHz_Plan160_2FSK_3_CN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('CN', 0x05, 160, 200000, 470200000, 'Wi-SUN FAN, CN 470-510 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan64_2FSK_1b_HK(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('HK', 0x02, 64, 200000, 920200000, 'Wi-SUN FAN, HK 920-925 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan64_2FSK_2a_HK(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('HK', 0x03, 64, 200000, 920200000, 'Wi-SUN FAN, HK 920-925 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_3_HK(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('HK', 0x05, 65, 400000, 920400000, 'Wi-SUN FAN, HK 920-925 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_4a_HK(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('HK', 0x06, 65, 400000, 920400000, 'Wi-SUN FAN, HK 920-925 MHz, Mode4a (2FSK 200kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_5_HK(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('HK', 0x08, 65, 400000, 920400000, 'Wi-SUN FAN, HK 920-925 MHz, Mode5 (2FSK 300kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_917MHz_Plan96_2FSK_1b_KR(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('KR', 0x02, 96, 200000, 917100000, 'Wi-SUN FAN, KR 917-923 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_917MHz_Plan96_2FSK_2a_KR(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('KR', 0x03, 96, 200000, 917100000, 'Wi-SUN FAN, KR 917-923 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_917MHz_Plan97_2FSK_3_KR(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('KR', 0x05, 97, 400000, 917300000, 'Wi-SUN FAN, KR 917-923 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_917MHz_Plan97_2FSK_4a_KR(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('KR', 0x06, 97, 400000, 917300000, 'Wi-SUN FAN, KR 917-923 MHz, Mode4a (2FSK 200kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_917MHz_Plan97_2FSK_5_KR(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('KR', 0x08, 97, 400000, 917300000, 'Wi-SUN FAN, KR 917-923 MHz, Mode5 (2FSK 300kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_902MHz_Plan1_2FSK_1b_MX(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('MX', 0x02, 1, 200000, 902200000, 'Wi-SUN FAN, MX 902-928 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_902MHz_Plan1_2FSK_2a_MX(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('MX', 0x03, 1, 200000, 902200000, 'Wi-SUN FAN, MX 902-928 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_902MHz_Plan2_2FSK_3_MX(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('MX', 0x05, 2, 400000, 902400000, 'Wi-SUN FAN, MX 902-928 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_902MHz_Plan2_2FSK_4a_MX(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('MX', 0x06, 2, 400000, 902400000, 'Wi-SUN FAN, MX 902-928 MHz, Mode4a (2FSK 200kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_902MHz_Plan2_2FSK_5_MX(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('MX', 0x08, 2, 400000, 902400000, 'Wi-SUN FAN, MX 902-928 MHz, Mode5 (2FSK 300kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_919MHz_Plan80_2FSK_1b_MY(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('MY', 0x02, 80, 200000, 919200000, 'Wi-SUN FAN, MY 919-923 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_919MHz_Plan80_2FSK_2a_MY(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('MY', 0x03, 80, 200000, 919200000, 'Wi-SUN FAN, MY 919-923 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_919MHz_Plan81_2FSK_3_MY(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('MY', 0x05, 81, 400000, 919400000, 'Wi-SUN FAN, MY 919-923 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_919MHz_Plan81_2FSK_4a_MY(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('MY', 0x06, 81, 400000, 919400000, 'Wi-SUN FAN, MY 919-923 MHz, Mode4a (2FSK 200kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_919MHz_Plan81_2FSK_5_MY(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('MY', 0x08, 81, 400000, 919400000, 'Wi-SUN FAN, MY 919-923 MHz, Mode5 (2FSK 300kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_915MHz_Plan48_2FSK_1b_PH(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('PH', 0x02, 48, 200000, 915200000, 'Wi-SUN FAN, PH 915-918 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_915MHz_Plan48_2FSK_2a_PH(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('PH', 0x03, 48, 200000, 915200000, 'Wi-SUN FAN, PH 915-918 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_915MHz_Plan49_2FSK_3_PH(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('PH', 0x05, 49, 400000, 915400000, 'Wi-SUN FAN, PH 915-918 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_915MHz_Plan49_2FSK_4a_PH(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('PH', 0x06, 49, 400000, 915400000, 'Wi-SUN FAN, PH 915-918 MHz, Mode4a (2FSK 200kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_915MHz_Plan49_2FSK_5_PH(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('PH', 0x08, 49, 400000, 915400000, 'Wi-SUN FAN, PH 915-918 MHz, Mode5 (2FSK 300kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_866MHz_Plan32_2FSK_1a_SG(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('SG', 0x01, 32, 100000, 866100000, 'Wi-SUN FAN, SG 866-869 MHz, Mode1a (2FSK 50kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_866MHz_Plan33_2FSK_2a_SG(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('SG', 0x03, 33, 200000, 866100000, 'Wi-SUN FAN, SG 866-869 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_866MHz_Plan33_2FSK_3_SG(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('SG', 0x05, 33, 200000, 866100000, 'Wi-SUN FAN, SG 866-869 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan64_2FSK_1b_SG(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('SG', 0x02, 64, 200000, 920200000, 'Wi-SUN FAN, SG 920-925 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan64_2FSK_2a_SG(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('SG', 0x03, 64, 200000, 920200000, 'Wi-SUN FAN, SG 920-925 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_3_SG(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('SG', 0x05, 65, 400000, 920400000, 'Wi-SUN FAN, SG 920-925 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_4a_SG(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('SG', 0x06, 65, 400000, 920400000, 'Wi-SUN FAN, SG 920-925 MHz, Mode4a (2FSK 200kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_5_SG(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('SG', 0x08, 65, 400000, 920400000, 'Wi-SUN FAN, SG 920-925 MHz, Mode5 (2FSK 300kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan64_2FSK_1b_TH(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('TH', 0x02, 64, 200000, 920200000, 'Wi-SUN FAN, TH 920-925 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan64_2FSK_2a_TH(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('TH', 0x03, 64, 200000, 920200000, 'Wi-SUN FAN, TH 920-925 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_3_TH(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('TH', 0x05, 65, 400000, 920400000, 'Wi-SUN FAN, TH 920-925 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_4a_TH(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('TH', 0x06, 65, 400000, 920400000, 'Wi-SUN FAN, TH 920-925 MHz, Mode4a (2FSK 200kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_5_TH(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('TH', 0x08, 65, 400000, 920400000, 'Wi-SUN FAN, TH 920-925 MHz, Mode5 (2FSK 300kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan64_2FSK_1b_VN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('VN', 0x02, 64, 200000, 920200000, 'Wi-SUN FAN, VN 920-925 MHz, Mode1b (2FSK 50kbps mi=1.0)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan64_2FSK_2a_VN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('VN', 0x03, 64, 200000, 920200000, 'Wi-SUN FAN, VN 920-925 MHz, Mode2a (2FSK 100kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_3_VN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('VN', 0x05, 65, 400000, 920400000, 'Wi-SUN FAN, VN 920-925 MHz, Mode3 (2FSK 150kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_4a_VN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('VN', 0x06, 65, 400000, 920400000, 'Wi-SUN FAN, VN 920-925 MHz, Mode4a (2FSK 200kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    def PHY_WISUN_FAN_1v1_920MHz_Plan65_2FSK_5_VN(self, model, phy_name=None):
        params = WisunFan1v1ChannelParamsJumbo('VN', 0x08, 65, 400000, 920400000, 'Wi-SUN FAN, VN 920-925 MHz, Mode5 (2FSK 300kbps mi=0.5)')
        phy = self._make_wisun_fan_phy(model, params, phy_name=phy_name)
        return phy
    
    pass