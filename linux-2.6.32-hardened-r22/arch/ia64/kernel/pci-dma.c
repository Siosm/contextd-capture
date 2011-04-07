/*
 * Dynamic DMA mapping support.
 */

#include <linux/types.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/pci.h>
#include <linux/module.h>
#include <linux/dmar.h>
#include <asm/iommu.h>
#include <asm/machvec.h>
#include <linux/dma-mapping.h>

#include <asm/system.h>

#ifdef CONFIG_DMAR

#include <linux/kernel.h>

#include <asm/page.h>

dma_addr_t bad_dma_address __read_mostly;
EXPORT_SYMBOL(bad_dma_address);

static int iommu_sac_force __read_mostly;

int no_iommu __read_mostly;
#ifdef CONFIG_IOMMU_DEBUG
int force_iommu __read_mostly = 1;
#else
int force_iommu __read_mostly;
#endif

int iommu_pass_through;

/* Dummy device used for NULL arguments (normally ISA). Better would
   be probably a smaller DMA mask, but this is bug-to-bug compatible
   to i386. */
struct device fallback_dev = {
	.init_name = "fallback device",
	.coherent_dma_mask = DMA_BIT_MASK(32),
	.dma_mask = &fallback_dev.coherent_dma_mask,
};

extern const struct dma_map_ops intel_dma_ops;

static int __init pci_iommu_init(void)
{
	if (iommu_detected)
		intel_iommu_init();

	return 0;
}

/* Must execute after PCI subsystem */
fs_initcall(pci_iommu_init);

void pci_iommu_shutdown(void)
{
	return;
}

void __init
iommu_dma_init(void)
{
	return;
}

int iommu_dma_supported(struct device *dev, u64 mask)
{
	/* Copied from i386. Doesn't make much sense, because it will
	   only work for pci_alloc_coherent.
	   The caller just has to use GFP_DMA in this case. */
	if (mask < DMA_BIT_MASK(24))
		return 0;

	/* Tell the device to use SAC when IOMMU force is on.  This
	   allows the driver to use cheaper accesses in some cases.

	   Problem with this is that if we overflow the IOMMU area and
	   return DAC as fallback address the device may not handle it
	   correctly.

	   As a special case some controllers have a 39bit address
	   mode that is as efficient as 32bit (aic79xx). Don't force
	   SAC for these.  Assume all masks <= 40 bits are of this
	   type. Normally this doesn't make any difference, but gives
	   more gentle handling of IOMMU overflow. */
	if (iommu_sac_force && (mask >= DMA_BIT_MASK(40))) {
		dev_info(dev, "Force SAC with mask %llx\n", mask);
		return 0;
	}

	return 1;
}
EXPORT_SYMBOL(iommu_dma_supported);

extern void *intel_alloc_coherent(struct device *hwdev, size_t size, dma_addr_t *dma_handle, gfp_t flags);
extern void intel_free_coherent(struct device *hwdev, size_t size, void *vaddr, dma_addr_t dma_handle);
extern int intel_map_sg(struct device *hwdev, struct scatterlist *sglist, int nelems, enum dma_data_direction dir, struct dma_attrs *attrs);
extern void intel_unmap_sg(struct device *hwdev, struct scatterlist *sglist, int nelems, enum dma_data_direction dir, struct dma_attrs *attrs);
extern dma_addr_t intel_map_page(struct device *dev, struct page *page, unsigned long offset, size_t size, enum dma_data_direction dir, struct dma_attrs *attrs);
extern void intel_unmap_page(struct device *dev, dma_addr_t dev_addr, size_t size, enum dma_data_direction dir, struct dma_attrs *attrs);
extern int intel_mapping_error(struct device *dev, dma_addr_t dma_addr);

static const struct dma_map_ops intel_iommu_dma_ops = {
	/* from drivers/pci/intel-iommu.c:intel_dma_ops */
	.alloc_coherent = intel_alloc_coherent,
	.free_coherent = intel_free_coherent,
	.map_sg = intel_map_sg,
	.unmap_sg = intel_unmap_sg,
	.map_page = intel_map_page,
	.unmap_page = intel_unmap_page,
	.mapping_error = intel_mapping_error,

	.sync_single_for_cpu = machvec_dma_sync_single,
	.sync_sg_for_cpu = machvec_dma_sync_sg,
	.sync_single_for_device = machvec_dma_sync_single,
	.sync_sg_for_device = machvec_dma_sync_sg,
	.dma_supported = iommu_dma_supported,
};

void __init pci_iommu_alloc(void)
{
	dma_ops = &intel_iommu_dma_ops;

	/*
	 * The order of these functions is important for
	 * fall-back/fail-over reasons
	 */
	detect_intel_iommu();

#ifdef CONFIG_SWIOTLB
	pci_swiotlb_init();
#endif
}

#endif
