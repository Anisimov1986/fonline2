#ifndef __FONLINE_LOCATIONEX__
#define __FONLINE_LOCATIONEX__

// inlined in fonline_tla.cpp

#define LOCEX_BUFFER_SIZE	(1024)
#define APPROX_GRID_THRESHOLD	(5)

class LocationEx
{
public:
	bool IsTown;
	bool IsGuarded;
	bool IsTC;

	bool IsTent;
	bool IsBase;
	bool IsHotel;

	bool IsQuest;
	bool IsEncounter;
	bool IsDungeon;
	bool IsRespawn;
    bool IsSpecial;
    bool IsEvent;
	bool IsSimulation;
	
	LocationEx();
	~LocationEx();
};

LocationEx::LocationEx()
{
	IsTown=false;
	IsGuarded=false;
	IsTC=false;

	IsTent=false;
	IsBase=false;
	IsHotel=false;

	IsQuest=false;
	IsEncounter=false;
	IsDungeon=false;
	IsRespawn=false;
    IsSpecial=false;
    IsEvent=false;
    IsSimulation=false;
}

LocationEx::~LocationEx() {}

struct GridCarrier
{
	vector<MapObjectVec> ToWM;
	vector<MapObjectVec> ToMap;
};

vector<LocationEx> LocationExArray;
typedef map<uint16, GridCarrier> GridMap;
GridMap GridInfo;

EXPORT void InitLocationEx()
{
  char buffer[LOCEX_BUFFER_SIZE];
  char *arg;
  char *param;
  char *area = "[Area";
  char *prefix = "#@";

  // add strings here
  char *IsTown="IsTown";
  char *IsGuarded="IsGuarded";
  char *IsTC="IsTC";

  char *IsTent="IsTent";
  char *IsBase="IsBase";
  char *IsHotel="IsHotel";

  char *IsQuest="IsQuest";
  char *IsEncounter="IsEncounter";
  char *IsDungeon="IsDungeon";
  char *IsRespawn="IsRespawn";
  char *IsSpecial="IsSpecial";
  char *IsEvent="IsEvent";
  char *IsSimulation="IsSimulation";
  
  int curpid=0;
  FILE *f=fopen("maps/Locations.cfg","r");
  if (!f) return;
  while (fgets(buffer, LOCEX_BUFFER_SIZE, f)!=NULL)
  {
    if (strncmp(area,buffer,5)==0)
    {
      int i=6;
      while (('0' <= buffer[i]) && (buffer[i] <= '9')) i++;
      buffer[i]=0;
      curpid=atoi(buffer+6);
      if ((uint)curpid>=LocationExArray.size()) LocationExArray.resize(curpid+1);
    }
    else
    if (strncmp(prefix,buffer,2)==0)
    {
      arg=buffer+2;
      int i=0;
      while (arg[i]!=0)
      {
        if (arg[i]=='=') { arg[i]=0; param=arg+(++i);}
        else
        if ((arg[i]==10) || (arg[i]==10)) arg[i++]=0;
        else
        i++;
      };

      // process arg and param here
      if (strcmp(IsTown,arg)==0) LocationExArray[curpid].IsTown = (atoi(param) != 0);
      else
      if (strcmp(IsGuarded,arg)==0) LocationExArray[curpid].IsGuarded = (atoi(param) != 0);
      else
      if (strcmp(IsTC,arg)==0) LocationExArray[curpid].IsTC = (atoi(param) != 0);
      else
      if (strcmp(IsTent,arg)==0) LocationExArray[curpid].IsTent = (atoi(param) != 0);
      else
      if (strcmp(IsBase,arg)==0) LocationExArray[curpid].IsBase = (atoi(param) != 0);
      else
      if (strcmp(IsHotel,arg)==0) LocationExArray[curpid].IsHotel = (atoi(param) != 0);
      else
      if (strcmp(IsQuest,arg)==0) LocationExArray[curpid].IsQuest = (atoi(param) != 0);
      else
      if (strcmp(IsEncounter,arg)==0) LocationExArray[curpid].IsEncounter = (atoi(param) != 0);
      else
      if (strcmp(IsDungeon,arg)==0) LocationExArray[curpid].IsDungeon = (atoi(param) != 0);
      else
      if (strcmp(IsRespawn,arg)==0) LocationExArray[curpid].IsRespawn = (atoi(param) != 0);
      else
      if (strcmp(IsSpecial,arg)==0) LocationExArray[curpid].IsSpecial = (atoi(param) != 0);
      else
      if (strcmp(IsEvent,arg)==0) LocationExArray[curpid].IsEvent = (atoi(param) != 0);
	  else
      if (strcmp(IsSimulation,arg)==0) LocationExArray[curpid].IsSimulation = (atoi(param) != 0);
      else
      Log( "LocationEx : unknown identifier<%s>\n", arg );
    }
  }
  fclose(f);
}

EXPORT void FinishLocationEx()
{
	LocationExArray.clear();
	GridInfo.clear();
}

EXPORT bool Location_IsTown(Location& loc) { return LocationExArray[loc.Data.LocPid].IsTown; }
EXPORT bool Location_IsGuarded(Location& loc) { return LocationExArray[loc.Data.LocPid].IsGuarded; }
EXPORT bool Location_IsTC(Location& loc) { return LocationExArray[loc.Data.LocPid].IsTC; }

EXPORT bool Location_IsTent(Location& loc) { return LocationExArray[loc.Data.LocPid].IsTent; }
EXPORT bool Location_IsBase(Location& loc) { return LocationExArray[loc.Data.LocPid].IsBase; }
EXPORT bool Location_IsHotel(Location& loc) { return LocationExArray[loc.Data.LocPid].IsHotel; }

EXPORT bool Location_IsQuest(Location& loc) { return LocationExArray[loc.Data.LocPid].IsQuest; }
EXPORT bool Location_IsEncounter(Location& loc) { return LocationExArray[loc.Data.LocPid].IsEncounter; }
EXPORT bool Location_IsDungeon(Location& loc) { return LocationExArray[loc.Data.LocPid].IsDungeon; }
EXPORT bool Location_IsRespawn(Location& loc) { return LocationExArray[loc.Data.LocPid].IsRespawn; }
EXPORT bool Location_IsSpecial(Location& loc) { return LocationExArray[loc.Data.LocPid].IsSpecial; }
EXPORT bool Location_IsEvent(Location& loc) { return LocationExArray[loc.Data.LocPid].IsEvent; }
EXPORT bool Location_IsSimulation(Location& loc) { return LocationExArray[loc.Data.LocPid].IsSimulation; }
MapObjectVec* FindNearestGroup(vector<MapObjectVec>& groups, uint16 hx, uint16 hy)
{
	if(groups.empty()) return NULL;

	auto cur_group = groups.begin();
	auto closest = cur_group;
	uint16 closest_dist = GetDistantion(hx, hy, cur_group->front()->MapX, cur_group->front()->MapY);
	if(!closest_dist) return closest;
	for(++cur_group; cur_group != groups.end(); ++cur_group)
	{
		uint16 dist = GetDistantion(hx, hy, cur_group->front()->MapX, cur_group->front()->MapY);
		if(dist < closest_dist)
		{
			closest_dist = dist;
			closest = cur_group;
			if(!closest_dist) break; // nothing can be closer
		}
	}
	return closest;
}

MapObject* FindNearestObject(const MapObjectVec& vec, uint16 hx, uint16 hy, bool withPid = false, uint16 toMapPid = 0)
{
	if(vec.empty()) return NULL;
	auto closest = vec.begin();
	auto curr = closest;
	uint16 min_dist = GetDistantion(hx, hy, (*closest)->MapX, (*closest)->MapY);
	if(!min_dist) return *closest;
	for(++curr; curr != vec.end(); ++curr)
	{
		MapObject* obj = *curr;
		if(withPid && obj->MScenery.ToMapPid != toMapPid) continue;

		uint16 dist = GetDistantion(hx, hy, obj->MapX, obj->MapY);
		if(dist < min_dist)
		{
			if(!dist) return *curr;
			min_dist = dist;
			closest = curr;
		}
	}
	return *closest;
}

GridCarrier& ProcessCarrier(const ProtoMap* proto)
{
	GridCarrier carrier;

	// first pass: create initial grids
	for(auto it = proto->GridsVec.begin(), end = proto->GridsVec.end(); it != end; ++it)
	{
		MapObject* obj = *it;
		uint16 hx = obj->MapX;
		uint16 hy = obj->MapY;

		vector<MapObjectVec>& groups = obj->MScenery.ToMapPid ? carrier.ToMap : carrier.ToWM;

		// is it closer than threshold value to some previously chosen initial?
		auto comparator = [&](const MapObjectVec& group) -> bool
		{
			MapObject* o = group.front();
			return GetDistantion(hx, hy, o->MapX, o->MapY) < APPROX_GRID_THRESHOLD;
		};

		bool is_close = find_if(groups.begin(), groups.end(), comparator) != groups.end();
		if(is_close) continue;

		// make a new initial and its group
		groups.push_back(MapObjectVec());
		groups.back().push_back(obj);
	}

	// second pass: assign grids to initials
	for(auto it = proto->GridsVec.begin(), end = proto->GridsVec.end(); it != end; ++it)
	{
		MapObject* obj = *it;
		uint16 hx = obj->MapX;
		uint16 hy = obj->MapY;

		vector<MapObjectVec>& groups = obj->MScenery.ToMapPid ? carrier.ToMap : carrier.ToWM;
		MapObjectVec* closest = FindNearestGroup(groups, hx, hy);
		uint16 closest_dist = GetDistantion(hx, hy, closest->front()->MapX, closest->front()->MapY);
		if(!closest_dist) continue; // is initial
		closest->push_back(obj);
	}

	GridInfo[proto->Pid] = carrier;
	return GridInfo[proto->Pid];
}

GridCarrier& GetCarrier(const ProtoMap* proto)
{
	GridMap::iterator it = GridInfo.find(proto->Pid);
	if(it == GridInfo.end()) return ProcessCarrier(proto);
	return it->second;
}

EXPORT bool Map_FindNearestGridRough(Map& imap, uint16& hx, uint16& hy, bool toWM)
{
	GridCarrier& carrier = GetCarrier(imap.Proto);

	vector<MapObjectVec>& groups = toWM ? carrier.ToWM : carrier.ToMap;
	MapObjectVec* vec = FindNearestGroup(groups, hx, hy);
	if(!vec) return false;
	hx = vec->front()->MapX;
	hy = vec->front()->MapY;
	return true;
}

EXPORT bool Map_FindNearestGridApprox(Map& imap, uint16& hx, uint16& hy, bool toWM)
{
	GridCarrier& carrier = GetCarrier(imap.Proto);

	vector<MapObjectVec>& groups = toWM ? carrier.ToWM : carrier.ToMap;
	MapObjectVec* vec = FindNearestGroup(groups, hx, hy);
	if(!vec) return false;

	MapObject* obj = FindNearestObject(*vec, hx, hy);
	hx = obj->MapX;
	hy = obj->MapY;
	return true;
}

EXPORT bool Map_FindNearestGrid(Map& imap, uint16& hx, uint16& hy, uint16 toMapPid)
{
	MapObject* obj = FindNearestObject(imap.Proto->GridsVec, hx, hy, true, toMapPid);
	if(!obj) return false;
	hx=obj->MapX;
	hy=obj->MapY;
	return true;
}

#endif // __FONLINE_LOCATIONEX__ //
