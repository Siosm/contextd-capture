#include "nsIGenericFactory.h"
#include "PIGAContext_Impl.h"
	
NS_GENERIC_FACTORY_CONSTRUCTOR(PIGAContextImpl)

static const nsModuleComponentInfo composants[] =
{
  {
    "Turns your firefox into a secured PIGA-aware software.",          // Description
    PIGACONTEXT_IID,        // Component dd
    PIGACONTEXT_CONTRACTID, // Contract id
    PIGAContextImplConstructor
  }
};
	
NS_IMPL_NSGETMODULE(PIGAContextImpl, composants)
