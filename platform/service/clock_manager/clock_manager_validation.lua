local hfxo_enable = slc.config("SL_CLOCK_MANAGER_HFXO_EN")
local lfxo_enable = slc.config("SL_CLOCK_MANAGER_LFXO_EN")
local dpll_enable = slc.config("SL_CLOCK_MANAGER_HFRCO_DPLL_EN")
local dpll_refclk = slc.config("SL_CLOCK_MANAGER_DPLL_REFCLK")
local lfrco_precision_enable = slc.config("SL_CLOCK_MANAGER_LFRCO_PRECISION")
local hf_default_clock_source = slc.config("SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE")
local lf_default_clock_source = slc.config("SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE")
local sysclk_source = slc.config("SL_CLOCK_MANAGER_SYSCLK_SOURCE")
local em01grpaclk_source = slc.config("SL_CLOCK_MANAGER_EM01GRPACLK_SOURCE")
local em01grpbclk_source = slc.config("SL_CLOCK_MANAGER_EM01GRPBCLK_SOURCE")
local em01grpcclk_source = slc.config("SL_CLOCK_MANAGER_EM01GRPCCLK_SOURCE")
local em01grpdclk_source = slc.config("SL_CLOCK_MANAGER_EM01GRPDCLK_SOURCE")
local em23grpaclk_source = slc.config("SL_CLOCK_MANAGER_EM23GRPACLK_SOURCE")
local em4grpaclk_source = slc.config("SL_CLOCK_MANAGER_EM4GRPACLK_SOURCE")
local pixelrzclk_source = slc.config("SL_CLOCK_MANAGER_PIXELRZCLK_SOURCE")
local qspisysclk_source = slc.config("SL_CLOCK_MANAGER_QSPISYSCLK_SOURCE")
local wdog0clk_source = slc.config("SL_CLOCK_MANAGER_WDOG0CLK_SOURCE")
local wdog1clk_source = slc.config("SL_CLOCK_MANAGER_WDOG1CLK_SOURCE")
local rtccclk_source = slc.config("SL_CLOCK_MANAGER_RTCCCLK_SOURCE")
local sysrtcclk_source = slc.config("SL_CLOCK_MANAGER_SYSRTCCLK_SOURCE")
local lcdclk_source = slc.config("SL_CLOCK_MANAGER_LCDCLK_SOURCE")
local eusartclk_source = slc.config("SL_CLOCK_MANAGER_EUSART0CLK_SOURCE")
local i2c0clk_source = slc.config("SL_CLOCK_MANAGER_I2C0CLK_SOURCE")
local hfxo_crystal_sharing = slc.config("SL_CLOCK_MANAGER_HFXO_CRYSTAL_SHARING_EN")
local hfxo_mode = slc.config("SL_CLOCK_MANAGER_HFXO_MODE")
local socpll_enable = slc.config("SL_CLOCK_MANAGER_SOCPLL_EN")
local socpll_refclk = slc.config("SL_CLOCK_MANAGER_SOCPLL_REFCLK")

-- OSCILLATORS VALIDATION --
-- HFXO related
if hfxo_enable.value == "0" and dpll_enable.value == "1" and dpll_refclk.value == "CMU_DPLLREFCLKCTRL_CLKSEL_HFXO" then
    validation.error(
    "DPLL module needs HFXO but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_DPLL_REFCLK"}),
    nil,
    nil)
end
if lfrco_precision_enable ~= nil then
  if hfxo_enable.value == "0" and (lfrco_precision_enable.value == "1" or lfrco_precision_enable.value == "cmuPrecisionHigh") then
    validation.error(
    "LFRCO High Precision Mode is enabled and requires HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_LFRCO_PRECISION"}),
    nil,
    nil)
  end
end

-- HFXO CRYSTAL SHARING
if hfxo_crystal_sharing ~= nil then
  if hfxo_crystal_sharing.value == "1" then
    local hfxo_crystal_sharing_leader = slc.config("SL_CLOCK_MANAGER_HFXO_CRYSTAL_SHARING_LEADER_EN")
    local hfxo_crystal_sharing_follower = slc.config("SL_CLOCK_MANAGER_HFXO_CRYSTAL_SHARING_FOLLOWER_EN")
    if hfxo_crystal_sharing_leader.value == "1" and hfxo_crystal_sharing_follower.value == "1" then
      validation.error(
        "Crystal sharing cannot be enabled for both the leader and follower",
        validation.target_for_defines({"SL_CLOCK_MANAGER_HFXO_CRYSTAL_SHARING_FOLLOWER_EN"}),
        nil,
        nil)
    end

    if hfxo_crystal_sharing_follower.value == "1" and hfxo_mode.value == "HFXO_CFG_MODE_XTAL" then
      validation.error(
        "HFXO mode must be HFXO_CFG_MODE_EXTCLKPKDET or HFXO_CFG_MODE_EXTCLK when crystal sharing follower is enabled",
        validation.target_for_defines({"SL_CLOCK_MANAGER_HFXO_MODE"}),
        nil,
        nil)
    end
  end
end

-- LFXO related
if lfxo_enable.value == "0" and dpll_enable.value == "1" and dpll_refclk.value == "CMU_DPLLREFCLKCTRL_CLKSEL_LFXO" then
  validation.error(
  "DPLL module needs LFXO but SL_CLOCK_MANAGER_LFXO_EN is disabled",
  validation.target_for_defines({"SL_CLOCK_MANAGER_DPLL_REFCLK"}),
  nil,
  nil)
end

-- DPLL related
 if dpll_enable.value == "1" then
  local dpll_n = tonumber(slc.config("SL_CLOCK_MANAGER_DPLL_N").value)
  local dpll_m = tonumber(slc.config("SL_CLOCK_MANAGER_DPLL_M").value)
  local dpll_freq_expected = tonumber(slc.config("SL_CLOCK_MANAGER_DPLL_FREQ").value)
  local dpll_refclk_freq
  if dpll_refclk.value == "CMU_DPLLREFCLKCTRL_CLKSEL_HFXO" then
    dpll_refclk_freq = tonumber(slc.config("SL_CLOCK_MANAGER_HFXO_FREQ").value)
  elseif dpll_refclk.value == "CMU_DPLLREFCLKCTRL_CLKSEL_LFXO" then
    dpll_refclk_freq = 32768
  elseif dpll_refclk.value == "CMU_DPLLREFCLKCTRL_CLKSEL_DISABLED" then
    validation.warning(
      "DPLL module is enabled but no Reference clock is selected",
      validation.target_for_defines({"SL_CLOCK_MANAGER_DPLL_REFCLK"}),
      nil,
      nil)
  end
  if dpll_refclk_freq ~= nil then
    -- check formula validation: dpll_freq = hfxo_freq * (N+1)/(M+1)
    local dpll_freq = dpll_refclk_freq * (dpll_n + 1) / (dpll_m + 1)
    if dpll_freq ~= dpll_freq_expected then
      validation.warning(
      "Target frequency is not reachable based on DPLL settings and Reference clock",
      validation.target_for_defines({"SL_CLOCK_MANAGER_DPLL_FREQ"}),
      nil,
      nil)
    end
  end
 end

 -- SOCPLL related
 if socpll_refclk ~= nil and socpll_enable ~= nil and socpll_enable.value == "1" then
  local socpll_refclk_freq
  if socpll_refclk.value == "SOCPLL_CTRL_REFCLKSEL_REF_HFXO" then
    if hfxo_enable.value == "0" then
      validation.error(
      "SOCPLL module needs HFXO but SL_CLOCK_MANAGER_HFXO_EN is disabled",
      validation.target_for_defines({"SL_CLOCK_MANAGER_SOCPLL_REFCLK"}),
      nil,
      nil)
    end
    socpll_refclk_freq = tonumber(slc.config("SL_CLOCK_MANAGER_HFXO_FREQ").value)
  elseif socpll_refclk.value == "SOCPLL_CTRL_REFCLKSEL_REF_HFRCO" then
    if dpll_enable.value == "1" then
      socpll_refclk_freq = tonumber(slc.config("SL_CLOCK_MANAGER_DPLL_FREQ").value)
    else
      socpll_refclk_freq = tonumber(slc.config("SL_CLOCK_MANAGER_HFRCO_BAND").value)
    end
  end
  if socpll_refclk_freq ~= nil then
    -- check formula validation: socpll_freq = Fref * (DIVN+2 + DIVF/1024) / 6
    local socpll_freq
    local socpll_freq_expected = tonumber(slc.config("SL_CLOCK_MANAGER_SOCPLL_FREQ").value)
    local socpll_fraq = slc.config("SL_CLOCK_MANAGER_SOCPLL_FRACTIONAL_EN")
    local socpll_divf = tonumber(slc.config("SL_CLOCK_MANAGER_SOCPLL_DIVF").value)
    local socpll_divn = tonumber(slc.config("SL_CLOCK_MANAGER_SOCPLL_DIVN").value)
    if socpll_fraq.value == "1" then
      socpll_freq = socpll_refclk_freq * (socpll_divn + 2 + socpll_divf/1024) / 6
    else
      socpll_freq = socpll_refclk_freq * (socpll_divn + 2) / 6
    end
    local socpll_max_ppm = 41
    local socpll_range_max = math.floor(socpll_freq + socpll_max_ppm * socpll_freq/1000000)
    local socpll_range_min = math.floor(socpll_freq - socpll_max_ppm * socpll_freq/1000000)
    if socpll_freq_expected < socpll_range_min or socpll_freq_expected > socpll_range_max then
      validation.warning(
      "Target frequency is not reachable based on SOCPLL settings and Reference clock",
      validation.target_for_defines({"SL_CLOCK_MANAGER_SOCPLL_FREQ"}),
      nil,
      nil)
    end
  end
end

-- CLOCK BRANCHES VALIDATION --
-- SYSCLK
if sysclk_source.value == "CMU_SYSCLKCTRL_CLKSEL_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "SYSCLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_SYSCLK_SOURCE"}),
    nil,
    nil)
end
if sysclk_source.value == "CMU_SYSCLKCTRL_CLKSEL_RFFPLL0SYS" and hfxo_enable.value == "0" then
  validation.error(
    "SYSCLK source branch is configured on RFFPLL, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_SYSCLK_SOURCE"}),
    nil,
    nil)
end
if sysclk_source.value == "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE" and hf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "SYSCLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_SYSCLK_SOURCE"}),
    nil,
    nil)
end
if sysclk_source.value == "CMU_SYSCLKCTRL_CLKSEL_SOCPLL" and socpll_enable ~= nil and socpll_enable.value == "0" then
  validation.error(
    "SYSCLK source branch is configured on SOCPLL, but SL_CLOCK_MANAGER_SOCPLL_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_SYSCLK_SOURCE"}),
    nil,
    nil)
end

-- EM01GRACLK
if em01grpaclk_source.value == "CMU_EM01GRPACLKCTRL_CLKSEL_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "EM01GRPACLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM01GRPACLK_SOURCE"}),
    nil,
    nil)
end
if em01grpaclk_source.value == "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE" and hf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "EM01GRPACLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM01GRPACLK_SOURCE"}),
    nil,
    nil)
end

-- EM01GRPBCLK
if em01grpbclk_source ~= nil then
if em01grpbclk_source.value == "CMU_EM01GRPBCLKCTRL_CLKSEL_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "EM01GRPBCLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM01GRPBCLK_SOURCE"}),
    nil,
    nil)
end
if em01grpbclk_source.value == "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE" and hf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "EM01GRPBCLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM01GRPBCLK_SOURCE"}),
    nil,
    nil)
end
end

-- EM01GRPCCLK
if em01grpcclk_source ~= nil then
if em01grpcclk_source.value == "CMU_EM01GRPCCLKCTRL_CLKSEL_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "EM01GRPCCLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM01GRPCCLK_SOURCE"}),
    nil,
    nil)
end
if em01grpcclk_source.value == "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE" and hf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "EM01GRPCCLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM01GRPCCLK_SOURCE"}),
    nil,
    nil)
end
end

-- EM01GRPDCLK
if em01grpdclk_source ~= nil then
if em01grpdclk_source.value == "CMU_EM01GRPDCLKCTRL_CLKSEL_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "EM01GRPDCLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM01GRPDCLK_SOURCE"}),
    nil,
    nil)
end
if em01grpdclk_source.value == "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE" and hf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "EM01GRPDCLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM01GRPDCLK_SOURCE"}),
    nil,
    nil)
end
end

-- PIXELRZCLK
if pixelrzclk_source ~= nil then
if pixelrzclk_source.value == "CMU_PIXELRZCLKCTRL_CLKSEL_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "PIXELRZCLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_PIXELRZCLK_SOURCE"}),
    nil,
    nil)
end
end

-- QSPISYSCLK
if qspisysclk_source ~= nil then
if qspisysclk_source.value == "CMU_QSPISYSCLKCTRL_CLKSEL_HFXO" and hfxo_enable.value == "0" then
  validation.error(
    "QSPISYSCLK source branch is configured on HFXO, but SL_CLOCK_MANAGER_HFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_QSPISYSCLK_SOURCE"}),
    nil,
    nil)
end
end

-- EM23GRPACLK
if em23grpaclk_source.value == "CMU_EM23GRPACLKCTRL_CLKSEL_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "EM23GRPACLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM23GRPACLK_SOURCE"}),
    nil,
    nil)
end
if em23grpaclk_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE" and lf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "EM23GRACLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM23GRPACLK_SOURCE"}),
    nil,
    nil)
end

-- EM4GRPACLK
if em4grpaclk_source.value == "CMU_EM4GRPACLKCTRL_CLKSEL_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "EM4GRPACLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM4GRPACLK_SOURCE"}),
    nil,
    nil)
end
if em4grpaclk_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE" and lf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "EM4GRACLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EM4GRPACLK_SOURCE"}),
    nil,
    nil)
end

-- WDOG0CLK
if wdog0clk_source.value == "CMU_WDOG0CLKCTRL_CLKSEL_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "WDOG0CLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_WDOG0CLK_SOURCE"}),
    nil,
    nil)
end
if wdog0clk_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE" and lf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "WDOG0CLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_WDOG0CLK_SOURCE"}),
    nil,
    nil)
end

-- WDOG1CLK
if wdog1clk_source ~= nil then
if wdog1clk_source.value == "CMU_WDOG1CLKCTRL_CLKSEL_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "WDOG1CLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_WDOG1CLK_SOURCE"}),
    nil,
    nil)
end
if wdog1clk_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE" and lf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "WDOG1CLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_WDOG1CLK_SOURCE"}),
    nil,
    nil)
end
end

-- RTCCCLK
if rtccclk_source ~= nil then
if rtccclk_source.value == "CMU_RTCCCLKCTRL_CLKSEL_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "RTCCCLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_RTCCCLK_SOURCE"}),
    nil,
    nil)
end
if rtccclk_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE" and lf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "RTCCCLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_RTCCCLK_SOURCE"}),
    nil,
    nil)
end
end

-- SYSRTCCLK
if sysrtcclk_source ~= nil then
if sysrtcclk_source.value == "CMU_SYSRTC0CLKCTRL_CLKSEL_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "SYSRTCCLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_SYSRTCCLK_SOURCE"}),
    nil,
    nil)
end
if sysrtcclk_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE" and lf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "SYSRTCCLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_SYSRTCCLK_SOURCE"}),
    nil,
    nil)
end
end

-- LCDCLK
if lcdclk_source ~= nil then
if lcdclk_source.value == "CMU_LCDCLKCTRL_CLKSEL_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "LCDCLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_LCDCLK_SOURCE"}),
    nil,
    nil)
end
if lcdclk_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE" and lf_default_clock_source.value == "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "LCDCLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_LCDCLK_SOURCE"}),
    nil,
    nil)
end
end

-- EUSART0CLK
if eusartclk_source ~= nil then
if eusartclk_source.value == "CMU_EUSART0CLKCTRL_CLKSEL_LFXO" and lfxo_enable.value == "0" then
  validation.error(
    "EUSART0CLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
    validation.target_for_defines({"SL_CLOCK_MANAGER_EUSART0CLK_SOURCE"}),
    nil,
    nil)
end
end

-- I2C0CLK
if i2c0clk_source ~= nil then
  if i2c0clk_source.value == "CMU_I2C0CLKCTRL_CLKSEL_LFXO" and lfxo_enable.value == "0" then
    validation.error(
      "I2C0CLK source branch is configured on LFXO, but SL_CLOCK_MANAGER_LFXO_EN is disabled",
      validation.target_for_defines({"SL_CLOCK_MANAGER_I2C0CLK_SOURCE"}),
      nil,
      nil)
  end
  end
