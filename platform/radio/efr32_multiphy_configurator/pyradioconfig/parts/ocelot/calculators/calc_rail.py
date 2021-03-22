from pyradioconfig.parts.lynx.calculators.calc_rail import CalcRailLynx

class CalcRailOcelot(CalcRailLynx):
    def calc_rail_delays(self, model):
        """calc_rail_delays

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        # Ocelot+ has a built-in RX chain delay value
        rx_delay_ns = int(round(model.vars.rx_grp_delay_us.value * 1000))
        sync_delay_ns = int((model.vars.MODEM_CTRL0_FRAMEDETDEL.value * 8 * 1e9) \
                            / model.vars.baudrate.value)

        startTx_delay_ns = int(round((model.vars.tx_grp_delay_us.value) * 1000)) # + on air delta ?
        endTx_delay_ns = 0

        # With no FRAMEDETDEL these values will be identical
        model.vars.rx_sync_delay_ns.value = rx_delay_ns + sync_delay_ns
        model.vars.rx_eof_delay_ns.value = rx_delay_ns

        model.vars.tx_sync_delay_ns.value = startTx_delay_ns
        model.vars.tx_eof_delay_ns.value = endTx_delay_ns

