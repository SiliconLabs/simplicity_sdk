local changeset = {}
-- Migration script for btmesh DMA channel configuration
if slc.is_provided("dmadrv") then
  local old_dma_count = slc.config('EMDRV_DMADRV_DMA_CH_COUNT')
  if old_dma_count ~= nil then
     if tonumber(old_dma_count.value) < 8 then
        -- If the old config is present and is not the maximum value, increase it by 1.
        table.insert(changeset, {
          ['option'] = 'EMDRV_DMADRV_DMA_CH_COUNT',
          ['value'] = tostring(tonumber(old_dma_count.value) + 1)
        })
     end
  end
end

return changeset
