/***************************************************************************************************

Copyright (c) 2015 Intellectual Ventures Property Holdings, LLC (IVPH) All rights reserved.

EMOD is licensed under the Creative Commons Attribution-Noncommercial-ShareAlike 4.0 License.
To view a copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode.

***************************************************************************************************/

#pragma once

#include <string>
#include <list>
#include <vector>

#include "BoostLibWrapper.h"

#include "EventCoordinator.h"
#include "Configure.h"
#include "NodeEventContext.h"

namespace Kernel
{
    struct ICampaignCostObserver; // TODO - Nasty fwd declaration because I'm scared to include NodeEventContext.h! :)

    // Standard distribution ec that just gives out the intervention once to the fraction of people specified by the coverage parameter
    class StandardInterventionDistributionEventCoordinator : public IEventCoordinator, public ITravelLinkedDistributionSource, public IVisitIndividual, public IEventCoordinator2, public JsonConfigurable
    {
        DECLARE_FACTORY_REGISTERED_EXPORT(EventCoordinatorFactory, StandardInterventionDistributionEventCoordinator, IEventCoordinator)    

    public:
        DECLARE_CONFIGURED(StandardInterventionDistributionEventCoordinator)
        IMPLEMENT_DEFAULT_REFERENCE_COUNTING()
        DECLARE_QUERY_INTERFACE()

        StandardInterventionDistributionEventCoordinator();
        virtual ~StandardInterventionDistributionEventCoordinator() { } 

        // IEventCoordinator
        virtual void SetContextTo(ISimulationEventContext *isec);
        
        virtual void AddNode( const suids::suid& node_suid);

        virtual void Update(float dt);
        virtual void UpdateNodes(float dt);
        virtual bool visitIndividualCallback(IIndividualHumanEventContext *ihec, float &incrementalCostOut, ICampaignCostObserver * pICCO );

        virtual bool IsFinished(); // returns true when the EC requires no further updates and can be disposed of

        // ITravelLinkedDistributionSource
        virtual void ProcessDeparting(IIndividualHumanEventContext *dc);
        virtual void ProcessArriving(IIndividualHumanEventContext *dc);

        virtual float GetDemographicCoverage() const;
        virtual TargetDemographicType::Enum GetTargetDemographic() const;
        virtual float GetMinimumAge() const;
        virtual float GetMaximumAge() const;

        virtual bool qualifiesDemographically( const IIndividualHumanEventContext * pIndividual ) const;

    protected:
        ISimulationEventContext  *parent;
        virtual float getDemographicCoverageForIndividual( const IIndividualHumanEventContext *pInd ) const;

        float coverage;
        bool distribution_complete;
        int num_repetitions;
        int tsteps_between_reps;
        float demographic_coverage;
        JsonConfigurable::tDynamicStringSet property_restrictions;
        std::map< std::string, std::string > property_restrictions_map;
        TargetDemographicType::Enum target_demographic;
        float target_age_min;
        float target_age_max;
        TargetGender::Enum target_gender; 
        int travel_linked; // should be bool
        int include_emigrants;
        int include_immigrants;
        int tsteps_since_last;
        int intervention_activated;
        InterventionConfig intervention_config;
        std::vector<INodeEventContext*> cached_nodes;
        std::vector<suids::suid> node_suids; // to help with serialization
        IDistributableIntervention *_di;

        // helpers
        void regenerateCachedNodeContextPointers();
        void formatInterventionClassNames( std::ostringstream&, json::QuickInterpreter*);
        virtual void initializeInterventionConfig( const Configuration * inputJson );
        virtual void validateInterventionConfig( const json::Element& rElement );
        virtual bool TargetedIndividualIsCovered(IIndividualHumanEventContext *ihec);
        bool property_restrictions_verified;

#if USE_JSON_SERIALIZATION
    public:
        // IJsonSerializable Interfaces
        virtual void JSerialize( IJsonObjectAdapter* root, JSerializer* helper ) const;
        virtual void JDeserialize( IJsonObjectAdapter* root, JSerializer* helper );
#endif    

#if USE_BOOST_SERIALIZATION
    private:
        friend class ::boost::serialization::access;
        template<class Archive>
        friend void serialize(Archive &ar, StandardInterventionDistributionEventCoordinator &ec, const unsigned int v);
#endif
    };
}