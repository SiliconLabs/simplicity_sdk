from pyradioconfig.parts.common.profiles.ocelot_regs import build_modem_regs_ocelot
from pyradioconfig.parts.ocelot.profiles.Profile_IEEE802154_OQPSK import ProfileIEEE802154OQPSKOcelot
from pyradioconfig.parts.sol.profiles.sw_profile_outputs_common import sw_profile_outputs_common_sol
from pycalcmodel.core.output import ModelOutput, ModelOutputType
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs


class ProfileIEEE802154OQPSKSol(ProfileIEEE802154OQPSKOcelot):

    def __init__(self):
        super().__init__()
        self._profileName = "IEEE802154OQPSK"
        self._readable_name = "IEEE802154 OQPSK Profile"
        self._category = ""
        self._description = "Profile used for IEEE802154 OQPSK phys"
        self._default = False
        self._activation_logic = ""
        self._family = "sol"
        self._sw_profile_outputs_common = sw_profile_outputs_common_sol()

    def build_info_profile_outputs(self, model, profile):
        super().build_info_profile_outputs(model, profile)
        profile.outputs.append(ModelOutput(model.vars.fpll_band, '', ModelOutputType.INFO, readable_name="RF Frequency Planning Band"))

    def build_register_profile_outputs(self, model, profile):
        build_modem_regs_ocelot(model, profile)

    def _build_modem_settings(self, model):
        super()._build_modem_settings(model)
        model.vars.directmode_rx.value_forced = None
