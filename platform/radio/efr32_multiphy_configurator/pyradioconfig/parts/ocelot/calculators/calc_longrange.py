from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum

class CALC_longrange_ocelot(ICalculator):

    def buildVariables(self, model):
        var = self._addModelVariable(model, 'longrange_mode', Enum, ModelVariableFormat.DECIMAL, 'Long Range Mode')
        member_data = [
            ['LR_1p2k', 0, 'OQPSK DSSS SF8 1.2k long range'],
            ['LR_2p4k', 1, 'OQPSK DSSS SF8 2.4k long range'],
            ['LR_4p8k', 2, 'OQPSK DSSS SF8 4.8k long range'],
            ['LR_9p6k', 3, 'OQPSK DSSS SF8 9.6k long range'],
            ['LR_19p2k', 4, 'OQPSK DSSS SF8 19.2k long range'],
            ['LR_38p4k', 5, 'OQPSK DSSS SF8 38.4k long range'],
            ['LR_80p0k', 6, 'OQPSK DSSS SF8 80.0k FCC 15.247 long range'],
            # Owner: Rossano Pantaleoni
            # Jira Link: https://jira.silabs.com/browse/MCUW_RADIO_CFG-2364
            ['LR_76p8k', 7, 'OQPSK DSSS SF8 76.8k long range'],
            ]
        var.var_enum = CreateModelVariableEnum(
            'LongRangeModeEnum',
            'List of supported LongRange Modes',
            member_data)