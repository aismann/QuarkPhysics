#ifndef QMESH_H
#define QMESH_H
#include <vector>
#include "cmath"
#include "qspring.h"
#include "qparticle.h"
#include "json/json.hpp"
#include "fstream"

using json =nlohmann::json;

using namespace std;

class QBody;
/**
 *@brief Every QBody object requires meshes. In other traditional physics engines, the term 'shape' is used instead. However, in Quark Physics, meshes do not only contain information about primitive shapes. A QMesh includes collision shapes, collision behaviors, internal and external spring connections, particle informations, and necessary collective information for rendering.
 * In the QMesh class, there are methods to quickly create primitive types such as circles, rectangles, and polygons that are suitable for the types and needs of body objects. However, QMesh objects are created with a struct called MeshData. Therefore, it is also possible to create many complex mesh examples for body types.
 */
struct QMesh
{
public:
	enum CollisionBehaviors{
		CIRCLES,
		POLYGONS,
		POLYLINE
	};
protected:
	vector<QParticle*> particles=vector<QParticle*>();

	//General Properties
	QVector position=QVector::Zero();
	QVector globalPosition=QVector::Zero();
	float rotation=0.0f;
	float globalRotation=0.0f;
	vector<QSpring*> springs=vector<QSpring*>();
	vector<vector<QParticle*>> closedPolygons=vector<vector<QParticle*>>();
	float circumference=0.0f;
	QBody *ownerBody=nullptr;
	CollisionBehaviors collisionBehavior=CollisionBehaviors::CIRCLES;

	//Helper Methods
	void UpdateCollisionBehavior();

public:
	/** The data struct of the mesh. 
	 */
	struct MeshData{
		/** The collection of local positions of particles. */
		vector<QVector> particlePositions;
		/** The collection of radius values of particles. */
		vector<float> particleRadValues;
		/** The collection of boolean values indicating whether a particle is internal. */
		vector<bool> particleInternalValues;
		/** The collection of integer pairs to define springs. 
		 * The integer values define the indices of particles in the particlePositions collection.  
		 * */
		vector<pair<int,int>> springList;
		/** The collection of integer pairs to define internal springs. 
		 * Internal springs are important for the some mass spring simulation configrations.
		 * The integer values define the indices of particles in the particlePositions collection.  
		 * */
		vector<pair<int,int>> internalSpringList;
		/** The polygon collection containing the index collection of the polygons.
		 * Closed polygons are important to define polygon colliders of the mesh.
		 * The particle orders should be clockwise.
		 * The integer values define the indices of particles in the particlePositions collection.  
		 * */
		vector <vector<int>> closedPolygonList;

		/** The position of the mesh */
		QVector position=QVector::Zero();

		/** The rotation of the mesh */
		float rotation=0.0f;
		
	};

	friend class QWorld;
	friend class QBody;
	friend class QRigidBody;
	friend class QSoftBody;

	/** Creates a mesh. */
	QMesh();
	~QMesh();


	//General Get Methods
	/** Returns the local position of the mesh. */
	QVector GetPosition(){
		return position;
	}
	/** Returns the global position of the mesh. */
	QVector GetGlobalPosition(){
		return globalPosition;
	}
	/** Returns the local rotation of the mesh. */
	float GetRotation(){
		return rotation;
	}
	/** Returns the global rotation of the mesh. */
	float GetGlobalRotation(){
		return globalRotation;
	}
	/** Returns the total area of the mesh with local positions of particles */
	float GetInitialArea(){
		float res=0.0f;
		for(auto poly:closedPolygons){
			res+=GetPolygonArea(poly,true);
		}
		for(auto particle:particles){
			if(particle->GetRadius()>0.5f){
				res+=particle->GetRadius()*particle->GetRadius();
			}
		}
		return res;
	}
	/** Returns the total polygon area of the mesh with local positions of particles */
	float GetInitialPolygonsArea(){
		float res=0.0f;
		for(auto poly:closedPolygons){
			res+=GetPolygonArea(poly,true);
		}
		return res;
	}
	/** Returns total area of the mesh with global positions of particles */

	float GetArea(){
		float res=0.0f;
		for(auto poly:closedPolygons){
			res+=GetPolygonArea(poly);
		}
		for(auto particle:particles){
			if(particle->GetRadius()>0.5f){
				res+=particle->GetRadius()*particle->GetRadius();
			}
		}
		return res;
	}
	/** Returns total polygon area of the mesh with global positions of particles */
	float GetPolygonsArea(){
		float res=0.0f;
		for(auto poly:closedPolygons){
			res+=GetPolygonArea(poly);
		}

		return res;
	}

	/** Returns total circumference of all polygons of the mesh (Calculates with local positions of particles) */
	float GetCircumference(){
		float res=0.0f;
		for(auto polygon:closedPolygons){
			for(int i=0;i<polygon.size();i++){
				QParticle *p=polygon[i];
				QParticle *np=polygon[(i+1)%polygon.size()];
				float length=(np->GetPosition()-p->GetPosition()).Length();
				res+=length;
			}
		}
//		for(auto spring:springs){
//			res+=(spring.GetParticleA()->GetGlobalPosition()-spring.GetParticleB()->GetGlobalPosition()).Length();
//		}
		return res;
	}
	/** Returns owner body of the mesh. 
	 * Owner body is the body in which the mesh is appointed. 
	 */
	QBody *GetOwnerBody(){
		return ownerBody;
	}
	/** Returns collision behaviors of the mesh. Collision behaviors can be circles, polygons, polylines.  
	 * The behaviors feature is important to determine collision methods for the mesh in the runtime.
	 * */
	CollisionBehaviors GetCollisionBehavior(){
		return collisionBehavior;
	}


	//General Set Methods
	/** Sets the local position of the mesh 
	 * @param value The local position value to set.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh* SetPosition(QVector value){
		position=value;
		return this;
	}
	/** Sets the global position of the mesh 
	 * @param value The global position value to set.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh* SetGlobalPosition(QVector value){
		globalPosition=value;
		return this;
	}
	/** Sets the rotation of the mesh 
	 * @param value The rotation value to set.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh *SetRotation(float value){
		rotation=value;
		return this;
	}
	




	//Particle Operations
	
	/** Adds a particle to the mesh
	 * @param particle A particle to be added.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh * AddParticle(QParticle *particle);

	/** Removes the particle from the mesh at the specified index.
	 * @param particle The index of particle to be removed.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh * RemoveParticleAt(int index);
	/** Removes a particle from the mesh
	 * @param particle A particle to be removed.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh * RemoveParticle(QParticle *particle);
	/** Returns the total particle count in the mesh.
	 */
	int GetParticleCount();
	/** Returns a particle at the specified index
	 * @param particle The index of particle to get.
	 */
	QParticle *GetParticle(int index);

	//Closed Polygons Operations

	/** Adds a polygon to the mesh
	 * @param polygon A particle pointers collection of the polygon.
	 */
	QMesh *AddClosedPolygon(vector<QParticle*> polygon);
	/** Removes a polygon from the mesh at the specified index.
	 * @param index The index of the polygon.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh *RemoveClosedPolygonAt(int index);

	/** Returns the total polygon count in the mesh.
	 */
	int GetClosedPolygonCount(){
		return closedPolygons.size();
	}
	/** Returns polygon at the specified index
	 * @param index The index of the polygon to get.
	 */
	vector<QParticle*> &GetClosedPolygon(int index){
		return closedPolygons[index];
	}
	/** Removes the polygons that contain the specified particle.
	 * @param particle A particle to be matched.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh *RemoveMatchingClosedPolygons(QParticle *particle);

	//Spring Operations
	
	/** Adds a spring to the mesh
	 * @param spring A spring to be added.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh *AddSpring(QSpring *spring);
	/** Removes a spring to the mesh
	 * @param spring A spring to be removed.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh *RemoveSpring(QSpring *spring);
	/** Removes a spring to the mesh at the specified index.
	 * @param index The index of the spring to be removed.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh *RemoveSpringAt(int index);
	/** Removes the springs that contain the specified particle.
	 * @param particle A particle to be matched.
	 * @return QMesh* A pointer to mesh itself.
	 */
	QMesh *RemoveMatchingSprings(QParticle *particle);
	/** Returns the total spring count in the mesh.
	 */
	int GetSpringCount(){
		return springs.size();
	}
	/** Returns the spring at the specified index.
	 * @param index The index of the spring to be get.
	 */
	QSpring *GetSpringAt(int index){
		return springs[index];
	}
	/** Returns the index of the specified spring.
	 * @param spring A spring in the mesh.
	 */
	int GetSpringIndex(QSpring *spring){
		for(int i=0;i<springs.size();i++)
			if(springs[i]==spring)
				return i;
		return -1;
	}






	//Static Methods
	/**
	 * @brief Creates a mesh with a circle shape.
	 * @param radius The radius of the circle
	 * @param centerPosition The center position of the circle in the mesh.
	 * @return QMesh* A pointer to the created mesh.
	 */
	static QMesh * CreateWithCircle(float radius,QVector centerPosition=QVector::Zero());
	/**
	 * @brief Creates a mesh with a convex polygon shape.
	 * @param radius The radius of the regular polygon
	 * @param sideCount The number of sides of the polygon
	 * @param centerPosition The center position of the polygon in the mesh.
	 * @param polarGrid Applies a polar grid to the polygon with the specified layer count.(It's unnecessary for rigid bodies) 
	 * @param enableSprings Allow springs to the polygon.(It's unnecessary for rigid bodies)
	 * @param enablePolygons Allow polygon colliders to the mesh. Set to false for PBD type soft body needs.
	 * @param particleRadius The radius of the particles
	 * @return QMesh* A pointer to the created mesh.
	 */
	static QMesh * CreateWithPolygon(float radius,int sideCount,QVector centerPosition=QVector::Zero(),int polarGrid=-1,bool enableSprings=true, bool enablePolygons=true,float particleRadius=0.5f);

	/**
	 * @brief Creates a mesh with a rectangle shape
	 * @param size The size of the rectangle
	 * @param centerPosition The center position of the rectangle in the mesh. 
	 * @param grid The grid property of the rectangle. Applies a grid to the rectangle.(It's unnecessary for rigid bodies)
	 * @param enableSprings Allow springs to the rectangle.(It's unnecessary for rigid bodies)
	 * @param enablePolygons Allows polygon colliders to the mesh. Set to false for PBD type soft body needs.  
	 * @param particleRadius The radius of the particles.
	 * @return QMesh* A pointer to the created mesh.
	 */
	static QMesh * CreateWithRect(QVector size,QVector centerPosition=QVector::Zero(),QVector grid=QVector::Zero(),bool enableSprings=true, bool enablePolygons=true,float particleRadius=0.5f);

	/** Creates a mesh with a specified mesh data.
	 * @param data Mesh data. 
	 * @param enableSprings Allow springs to the mesh.(It's unnecessary for rigid bodies)
	 * @param enablePolygons Allow polygon colliders to the mesh. Set to false for PBD type soft body needs.  
	 * @return QMesh* A pointer to the created mesh.
	 */
	static QMesh * CreateWithMeshData(QMesh::MeshData &data,bool enableSprings=true, bool enablePolygons=true);

	/**Returns mesh data list from a json based *.qmesh file.You can use the returned mesh data of the collection with the QWorld::CreateWithMeshData method.
	 * @param filePath The filePath to load
	 * @return vector<QMesh::MeshData> A Mesh data list. 
	 */
	static vector<QMesh::MeshData> GetMeshDatasFromFile(string filePath);





	/**
	 * @brief Returns the area of a polygon
	 * @param polygonPoints A collection of particle pointers of the polygon
	 * @param withLocalPositions Defines whether the calculations will be done with local positions. 
	 */
	static float GetPolygonArea(vector<QParticle*> &polygonPoints,bool withLocalPositions=false);


	/**
	 * @brief Checks collision behaviors between two bodies. If the specified body pair is the same as the specified collision behavior pair, returns true. 
	 * @param meshA A mesh to check
	 * @param meshB Another mesh to check
	 * @param firstBehavior A behavior to check
	 * @param secondBehavior Another behavior to check. 
	 */
	static bool CheckCollisionBehaviors(QMesh *meshA,QMesh * meshB,CollisionBehaviors firstBehavior,CollisionBehaviors secondBehavior );
	/**
	 * @brief Generates rectangle mesh data with specified properties.
	 * @param size Size of the rectangle.
	 * @param centerPosition The center position of the rectangle. 
	 * @param grid The grid property of the rectangle. Applies a grid to the rectangle.(It's unnecessary for rigid bodies) 
	 * @param particleRadius The radius of the particles.
	 * @return MeshData The data needed to create a mesh.
	 */
	static MeshData GenerateRectangleMeshData(QVector size,QVector centerPosition=QVector::Zero(),QVector grid=QVector::Zero(),float particleRadius=0.5f);
	/**
	 * @brief Generates a regular polygon mesh data with specified properties.
	 * @param radius The radius of the regular polygon.
	 * @param sideCount The number of sides of the polygon. 
	 * @param centerPosition The center position of the polygon.
	 * @param polarGrid Applies a polar grid to the polygon with the specified layer count.(It's unnecessary for rigid bodies)
	 * @param particleRadius The radius of the particles.
	 * @return MeshData The data needed to create a mesh.
	 */
	static MeshData GeneratePolygonMeshData(float radius, int sideCount, QVector centerPosition=QVector::Zero(),int polarGrid=-1,float particleRadius=0.5f);





};

#endif // QMESH_H