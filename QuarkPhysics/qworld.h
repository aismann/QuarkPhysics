#ifndef QWORLD_H
#define QWORLD_H
#include <iostream>
#include <map>
#include <vector>
#include "qbody.h"
#include "qjoint.h"
#include "qvector.h"
#include "qmanifold.h"
#include "qgizmos.h"
#include "qbroadphase.h"
#include "qcollision.h"
#include "qrigidbody.h"
#include "qsoftbody.h"
#include "qraycast.h"
#include "qjoint.h"


using namespace std;
/**
 * @brief A QWorld object is required to create a physics simulation. The QWorld class manages the entire physics simulation. You can add or remove objects to the physics world and make specific settings for the simulation. Additionally, the QWorld class is responsible for updating the simulation
 */
class QWorld{

	struct bodyPairHash {
		size_t operator()(const std::pair<QBody*, QBody*> &p) const {
			 return std::hash<QBody*>()(p.first) + std::hash<QBody*>()(p.second);
		}
	};

	struct bodyPairEqual {
		bool operator()(const std::pair<QBody*, QBody*> &p1, const std::pair<QBody*, QBody*> &p2) const {
			return ( (p1.first == p2.first && p1.second == p2.second) ) ;

		}
	};
protected:
	//Collections
	vector<QBody*> bodies=vector<QBody*>();

	vector <QJoint*> joints=vector<QJoint*>();

	vector <QSpring*> springs=vector<QSpring*>();

	vector<QRaycast*> raycasts=vector<QRaycast*>();

	vector <QGizmo*> gizmos=vector<QGizmo*>();

	vector<vector<QBody*> > sleepingIslands=vector<vector<QBody*> >();
	unordered_set<pair<QBody*, QBody*>, bodyPairHash,bodyPairEqual> collisionExceptions;

	QBroadPhase broadPhase=QBroadPhase(QVector(128.0f,128.0f));

	vector<QManifold> manifolds;


	//Physics World Properties

	QVector gravity=QVector(0.0f,0.2f);
	bool enableSleeping=true;
	bool enableBroadphase=true;
	int iteration=4;

	//Infographic Features
	int debugAABBTestCount=0; //aabb test method call count
	int debugCollisionTestCount=0; // any collision method call count

	void ClearGizmos();
	void ClearBodies();


	//Collisions, Dynamic Colliders Islands and Sleeping Feature
	void CreateIslands(QBody *body, vector<QBody*> &bodyList, vector<QBody*> &island);
	void GenerateIslands(vector<QBody*> &bodyList, vector<vector<QBody*>> &islandList);
	static bool SortBodies(const QBody *bodyA,const QBody *bodyB);
	static bool SortBodiesVertical(const QBody *bodyA,const QBody *bodyB);
	void GetCollisionPairs(vector<QBody *> &bodyList,vector<pair<QBody *, QBody *> > *resList);

	void CreateIslands(QBody &body, vector<QBody*> island);
	vector<vector<QBody>> GenerateIslands(vector<QBody> bodyList );

	//Constraints
	void UpdateConstraints();



public:
	/* It's the maximum world size in pixels. It is used in some calculations that require maximum values. */
	inline static float MAX_WORLD_SIZE=99999.0f;

	//General Get Methods
	/* Returns the gravity force of the world. 
	 */
	QVector GetGravity(){
		return gravity;
	}
	/** Returns whether sleep mode will be applied to dynamic objects.
	 */
	bool GetEnableSleeping(){
		return enableSleeping;
	}
	/** Returns whether broad phase optimization will be applied to the collisions. 
	 */
	bool GetEnableBroadphase(){
		return enableBroadphase;
	}
	/** Returns the iteration count per step of physics in the world. 
	 * The Iteration count determines the stability level of the simulation.
	 */
	int GetIterationCount(){
		return iteration;
	}

	//General Set Methods
	/** Sets the gravity force of the world.
	 * The gravity force applies to dynamic bodies in every step of physics.
	 * @param value The force vector value of the gravity.
	 */
	QWorld *SetGravity(QVector value){
		gravity=value;
		return this;
	}
	/** Sets whether sleep mode will be applied to dynamic objects.
	 * @param value True or false. 
	 */
	QWorld *SetEnableSleeping(bool value){
		enableSleeping=value;
		return this;
	}
	/** Sets whether broad phase optimization will be applied to the collisions.  
	 */
	QWorld *SetEnableBroadphase(bool value){
		enableBroadphase=value;
		return this;
	}
	/** Sets the iteration count per step of physics in the world. 
	 * Iteration count determines stability level of the simulation.
	 * @param value The number of iterations per step.
	 */
	QWorld *SetIterationCount(int value){
		iteration=value;
		return this;
	}

	//Methods
	/** Updates the physics simulation of the world as a step. 
	 */
	void Update();



	/**
	 * Performs the collision of the body and returns a contact list as a collision result. 
	 * @param bodyA A body in the world.
	 * @param bodyB Another body in the world.
	 */
	static vector<QCollision::Contact> GetCollisions(QBody *bodyA, QBody *bodyB);


	/**Adds a body to the world
	 * @param body A body to be added
	 */
	QWorld *AddBody(QBody *body);
	/**Adds a body group to the world
	 * @param bodyGroup A collection of bodies to be added
	 */
	QWorld *AddBodyGroup(const vector<QBody*> &bodyGroup);
	/**Removes body from the world.
	 * @param body A body to be removed.
	 */
	QWorld *RemoveBody(QBody *body);
	 /**Removes the body from the world at the specified index.
	  * The index of a body can be obtained using the GetBodyIndex() method.
	  * @param index The index of the body to remove.
	  */
	QWorld *RemoveBodyAt(int index);
	/** Returns the total body count in the world.
	 */
	int GetBodyCount(){
		return bodies.size();
	}
	/** Returns body at the specified index
	 * @param index The Index of the body to get.
	 */
	QBody *GetBodyAt(int index){
		return bodies[index];
	}
	/** Returns index of the specified body
	 * @param body A body pointer
	 */
	int GetBodyIndex(QBody *body){
		for(int i=0;i<bodies.size();i++)
			if(bodies[i]==body)
				return i;
		return -1;
	}
	/** Returns a list of bodies that collide with the point given.
	 * @param point A point to be collided.
	 * @param maxBodyCount The maximum number of bodies to be returned.
	 * @param onlyRigidBodies If set to true, it only tests rigid bodies.(It's true as a default)
	 * @param layersBit Collision layer bits(It setted -1 as a default,  it's mean it doesn't filter bodies via layer bits )
	 */
	vector<QBody*> GetBodiesHitByPoint(QVector point,int maxBodyCount=1,bool onlyRigidBodies=true,int layersBit=-1);
	/** Returns nearest particles to the specified point.
	 * @param point A point to test
	 * @param distance The distance of proximity to the point
	 * @param maxParticleCount The maximum number of particles to be returned.
	 * @param exceptRigidBodies If set to true, it tests only non-rigid bodies.
	 * @param layersBit Collision layer bits(It setted -1 as a default,  it's mean doesn't filter bodies via layer bits )
	 */
	vector<QParticle*> GetParticlesCloseToPoint(QVector point,float distance,int maxParticleCount=1,bool exceptRigidBodies=true,int layersBit=-1);
	/** Collides a body given to other bodies in the world. If is there a collision, it returns true.
	 * @param body A body from the world.
	 */
	bool CollideWithWorld(QBody *body);

	//Joint Operations
	/** Adds a joint to the world.
	 * @param joint A joint to be added
	 */
	QWorld *AddJoint(QJoint *joint);
	/** Removes a joint from the world.
	 * @param joint A joint to be removed.
	 */
	QWorld *RemoveJoint(QJoint *joint);
	/** Removes a joint from the world at the specified index. 
	 * The index of a joint can be obtained using the GetJointIndex() method.
	 * @param index The index of the joint to be removed. 
	 */
	QWorld *RemoveJointAt(int index);
	/** Removes the joints that contain the specified body.
	 * @param body A body to be matched.
	 */
	QWorld *RemoveMatchingJoints(QBody *body);
	/** Returns the total joint count of the world. 
	 */
	int GetJointCount(){
		return joints.size();
	}
	/** Returns the joint at the specified index.
	 * @param index The index of the joint.
	 */
	QJoint *GetJointAt(int index){
		return joints[index];
	}
	/** Returns the index of the specified joint.
	 * @param joint A joint in the world.
	 */
	int GetJointIndex(QJoint *joint){
		for(int i=0;i<joints.size();i++)
			if(joints[i]==joint)
				return i;
		return -1;
	}

	//Spring Operations
	/** Adds a spring to the world. 
	 * @param spring A spring to be added.
	 */
	QWorld *AddSpring(QSpring *spring);
	/** Removes a spring from the world.
	 * @param spring A spring to be removed. 
	 */
	QWorld *RemoveSpring(QSpring *spring);
	/** Removes a spring from the world at the specified index.
	 * The index of a spring can be obtained using the GetSpringIndex() method.
	 * @param index The index of the spring to be removed.
	 */
	QWorld *RemoveSpringAt(int index);
	/** Removes the springs that contain the specified body. 
	 * @param body A body to be matched.
	 */
	QWorld *RemoveMatchingSprings(QBody *body);
	/** Removes the springs that contain the specified particle.
	 * @param particle A particle to be matched.
	 */
	QWorld *RemoveMatchingSprings(QParticle *particle);
	/**Returns the total spring count of the world.
	 */
	int GetSpringCount(){
		return springs.size();
	}
	/** Returns the spring at the specified index.
	 * @param index The index of the spring.
	 */
	QSpring *GetSpringAt(int index){
		return springs[index];
	}
	/** Returns the index of the specified spring.
	 * @param spring A spring in the world.
	 */
	int GetSpringIndex(QSpring *spring){
		for(int i=0;i<springs.size();i++)
			if(springs[i]==spring)
				return i;
		return -1;
	}

	//Raycast Operations
	/** Adds a raycast to the world.
	 * @param raycast A raycast to be added.
	 */
	QWorld *AddRaycast(QRaycast * raycast);
	/** Removes a raycast from the world. 
	 * @param raycast A raycast to be removed.
	 */
	QWorld *RemoveRaycast(QRaycast *raycast);
	/** Removes a raycast from the world at the specified index. 
	 * The index of a raycast can be obtained using the GetRaycastIndex() method.
	 * @param index An index number.
	 */
	QWorld *RemoveRaycastAt(int index);
	/**Returns the total raycast count of the world. 
	 */
	int GetRaycastCount(){
		return raycasts.size();
	}
	/** Returns the raycast at the specified index.
	 * @param index The index of the raycast
	 */
	QRaycast *GetRaycastAt(int index){
		return raycasts[index];
	}
	/** Returns the index of the specified raycast.
	 * @param raycast A raycast in the world.
	 */
	int GetRaycastIndex(QRaycast *raycast){
		for(int i=0;i<raycasts.size();i++)
			if(raycasts[i]==raycast)
				return i;
		return -1;
	}

	//Gizmos Operations
	/**Returns the collection of gizmos in the world. 
	 */
	vector<QGizmo*> *GetGizmos(){
		return &gizmos;
	};

	//Collision Exceptions	
	/** Adds a collision exception between two bodies. These bodies will never collide.
	 *  @param bodyA A body in the world.
	 *  @param bodyB Another body in the world.
	 */
	QWorld *AddCollisionException(QBody *bodyA, QBody *bodyB);
	/** Removes a collision exception.
	 * @param bodyA A body in the world.
	 * @param bodyB Another body in the world.
	 */
	QWorld *RemoveCollisionException(QBody *bodyA, QBody *bodyB);
	/** Removes a collision exception that contains the given body.
	 * @param body A body to be matched.
	 */
	QWorld *RemoveMatchingCollisionException(QBody *body);
	/** Checks whether there is a collision exception between two bodies.
	 * @param bodyA A body in the world.
	 * @param bodyB Another body in the world.
	 */
	bool CheckCollisionException(QBody *bodyA, QBody *bodyB);

	//
	/** Creates a world. 
	 */
	QWorld();
	~QWorld();
	/** Removes all joints from the world. 
	 */
	QWorld *ClearJoints();
	/** Removes all springs from the world.
	 */
	QWorld *ClearSprings();
	/** It removes all raycasts from the world.
	 */
	QWorld *ClearRaycasts();
	/** It removes all objects from the world. 
	 */
	QWorld *ClearWorld();

	friend class QCollision;
	friend class QManifold;



};

#endif // QWORLD_H