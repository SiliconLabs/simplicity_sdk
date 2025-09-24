from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter

#
# Implements iphy_filter.py
#
class PhyFilters(IPhyFilter):

    customer_phy_groups = ['Phys_secret1',
                           'Phys_internal',
                           'Phys_virtual',
                           'Phys_virtual_studio'
                           ]
    # Special designation for non-functional PHYs
    non_functional_phy_groups = []
    sim_tests_phy_groups = ['Phys_test']
    simplicity_studio_phy_groups = []
    virtual_phy_groups = ['Phys_virtual', 'Phys_virtual_studio']

