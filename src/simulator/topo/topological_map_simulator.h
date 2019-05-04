/* Copyright (C) 2010-2019, The Regents of The University of Michigan.
 All rights reserved.

 This software was developed as part of the The Vulcan project in the Intelligent Robotics Lab
 under the direction of Benjamin Kuipers, kuipers@umich.edu. Use of this code is governed by an
 MIT-style License that can be found at "https://github.com/h2ssh/Vulcan".
*/


/**
* \file     topological_map_simulator.h
* \author   Collin Johnson
*
* Declaration of TopologicalMapSimulator.
*/

#ifndef SIMULATOR_TOPO_TOPOLOGICAL_MAP_SIMULATOR_H
#define SIMULATOR_TOPO_TOPOLOGICAL_MAP_SIMULATOR_H

#include <simulator/topo/params.h>
#include <hssh/global_topological/global_location.h>
#include <hssh/global_topological/topological_map.h>
#include <hssh/local_topological/local_topology_events.h>

namespace vulcan
{
namespace hssh    { class MetricMapCache; }
namespace planner { class DecisionTarget; }

namespace simulator
{

/**
* event_type_t details the type of event generated by a DecisionTarget.
*/
enum event_type_t
{
    LOCAL_PLACE_EVENT,
    LOCAL_PATH_EVENT,
    NO_EVENT
};

/**
* TopologicalMapSimulator simulates motion through a topological map by pretending to be
* the local topological layer of the HSSH. The simulator simulates the series of local
* place and path events that would be encountered by a robot moving through an environment.
*
* The simulator accepts a topological map and an initial robot position within the map. From
* here, the simulator moves the robot through the map based on DecisionTargets received from
* a global topological planner. For each target, a corresponding local_topology_place_event_t
* or local_topology_path_event_t is generated. These targets are based on the LocalPlaces stored
* with the map.
*
* The simulator handles a single target at a time. If a sequence of targets has been received,
* process them one at a time. After each update, the actual state of the robot in the map can
* be recovered by retrieving the simulated state. Thus, the actual position of the robot can be
* compared against the versions estimated by the global topological layer of the HSSH.
*
* In the future, the simulator will be updated to allow probabilistic models to be used for the
* actions and measurements. For now, everything is deterministic.
*
* There aren't any parameters at the moment. The parameters will be for the probabilistic version
* when it is implemented.
*/
class TopologicalMapSimulator
{
public:

    /**
    * Constructor for TopologicalMapSimulator.
    *
    * \param    params          Parameters for running the simulator
    * \param    manager         Place manager for loading the LocalPlaces to put in the place events
    */
    TopologicalMapSimulator(const topological_map_simulator_params_t& params, const hssh::MetricMapCache& manager);

    /**
    * setMapToSimulate sets the map being simulated and the initial state within the map.
    *
    * \param    map         Actual map through which motion will be simulated
    * \param    state       Initial state of the robot within the map
    */
    void setMapToSimulate(const hssh::TopologicalMap& map, const hssh::GlobalLocation& state);

    /**
    * setMapState changes the state of the robot within the map, allowing the robot to be kidnapped, for example.
    *
    * \param    state       New state of the robot in the map
    */
    void setMapState(const hssh::GlobalLocation& state);

    /**
    * simulateTarget simulates the changes in the local topology of the robot's surround if the provided
    * DecisionTarget were followed. A target can generate either a local_topology_place_event_t or a
    * local_topology_path_event_t. If the robot moves along a frontier in the map, that can't be simulated
    * so no event is generated.
    *
    * \param    target          Target to be simulated
    * \return   The type of event generated when executing the target.
    */
    event_type_t simulateTarget(const std::shared_ptr<planner::DecisionTarget>& target);

    /**
    * getPlaceEvent retrieves the most recently generated place event.
    */
    const hssh::local_topology_place_event_t& getPlaceEvent(void) const { return placeEvent; }

    /**
    * getPathEvent retrieves the most recently generated path event.
    */
    const hssh::local_path_fragment_t& getPathEvent(void) const { return pathEvent; }

    /**
    * getSimulatedState retrieves the state of the robot within the simulated map.
    */
    hssh::GlobalLocation getSimulatedState(void) const { return mapState; }

private:

    event_type_t simulatePlaceNeighborhoodTarget(const std::shared_ptr<planner::DecisionTarget>& target);
    event_type_t simulateRelativePlaceTarget    (const std::shared_ptr<planner::DecisionTarget>& target);
    event_type_t simulateLocalPathTarget        (const std::shared_ptr<planner::DecisionTarget>& target);

    hssh::TopologicalMap               topoMap;
    hssh::local_topology_place_event_t placeEvent;
    hssh::local_topology_path_event_t  pathEvent;
    hssh::GlobalLocation            mapState;

    const hssh::MetricMapCache& placeManager;

    topological_map_simulator_params_t params;
};

}
}

#endif // SIMULATOR_TOPO_TOPOLOGICAL_MAP_SIMULATOR_H