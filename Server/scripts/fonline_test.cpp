// Autor:[SOT]Skycast
#include "fonline_tla.h"
#ifdef __SERVER

#define BIAS_FLOAT     (0.02f)

FONLINE_DLL_ENTRY( isCompiler )
{
    if( isCompiler )
        return;

    // Test Memory Level 3 for loaded DLLs
    for( uint i = 0; i < 666; i++ )
        volatile char* leak = new char[ 2 ];
}
int  GetNightPersonBonus();
bool TraceWall(uint16 hx, uint16 hy, uint16 tx, uint16 ty, Map& map, int dist);
float GetDirectionF(uint16 hx, uint16 hy, uint16 tx, uint16 ty);
int GetDirLook(float dir, int crDir);
int GetDirNerf(int pe, int lookdir);
int GetDirSneakNerf(int fullpe, int lookdir, Critter& opponent);
EXPORT bool check_look(Map& map, Critter& cr, Critter& opponent)
{
	if (cr.Params[VISION_LEVEL]<opponent.Params[INVIS_LEVEL])return false; //Gm and other purposes
	if (cr.Params[VISION_LEVEL]>1)return true; //Gm and other purposes
	if (cr.Cond!=1)return false; //Dead/Ko/Kd critters cant see anything
	if ((uint) opponent.Params[ TO_SPAWN_TIMEOUT ] > FOnline->FullSecond) return false;
	int dist=GetDistantion(cr.HexX,cr.HexY,opponent.HexX,opponent.HexY); //Getting distandion between critters
	if (dist>3 && opponent.Params[PERMASNEAK]>0)return false;
	int pe = ( cr.Params[ DAMAGE_EYE ] ? 1 : cr.Params[ ST_PERCEPTION ] + cr.Params[ ST_PERCEPTION_EXT ] ); //Calculating pe
    //if( cr.Params[ TRAIT_NIGHT_PERSON ] )pe += GetNightPersonBonus(); //If nightperson + or - 1 pe DISABLED FOR NOW
    pe=CLAMP( pe, 1, 10 ); //Cant be less 1 and more 10
	pe=pe*3+20+cr.Params[ST_BONUS_LOOK] + cr.Params[ ST_BONUS_LOOK_EXT ] + cr.Params[ PE_LOOK_BONUS ] * 30; //Calculating view distance
	int fullpe=pe;
	int lookdir=0; //Npc see all critter like they locking direct to them
	if (!cr.CritterIsNpc) //Nerf look to players
	{
		float dir = GetDirectionF( cr.HexX, cr.HexY, opponent.HexX, opponent.HexY );
		lookdir=GetDirLook(dir,cr.Dir); //Getting direction how we look to opponent
		pe=GetDirNerf(pe,lookdir); //Nerfing pe for look purposes, stats in config.fos
	}
	if (dist>pe)return false; //If distantion to opponent more than our pr/nerfed pe we not see opponent
	bool motion_sensor=(cr.ItemSlotExt->Proto->ProtoId==208 || cr.ItemSlotMain->Proto->ProtoId==208);
	if (opponent.Params[ MODE_HIDE ] == 1) //If enemy in sneak
	{
		if (cr.Params[VISION_LEVEL]==1)return true; //For special npc's and other purposes.
		if (motion_sensor)
			lookdir=0;
		if (dist>GetDirSneakNerf(fullpe,lookdir,opponent))return false; //Nerfing sneak by look direction and weight
	}
	if (cr.CritterIsNpc)return true; // Npc see anyone not proccessed before
	if (cr.MapId != map.Data.MapId && map.Data.UserData[61]!=0) return false;
	if (motion_sensor && dist<15)return true;
	if (TraceWall(cr.HexX, cr.HexY, opponent.HexX, opponent.HexY, map, dist ))return false; //Tracing walls
	return true;
}

int GetNightPersonBonus() 
{
    if( FOnline->Hour < 6 || FOnline->Hour > 18 )
        return 1;
    else if( FOnline->Hour == 6 && FOnline->Minute == 0 )
        return 1;
    else if( FOnline->Hour == 18 && FOnline->Minute > 0 )
        return 1;
    return -1;
}

int GetDirNerf(int pe, int lookdir)
{
	double nerf=pe*(100-FOnline->LookDir[lookdir])/100; //Nerf look by dir from config.fos
	pe=(int) ceil(nerf); 
	return pe;
}

int GetDirSneakNerf(int fullpe, int lookdir, Critter& opponent)
{
	int sneakdist;
	int skill=opponent.Params[SK_SNEAK];
	int weight=opponent.GetItemsWeight();
	skill=CLAMP( skill, 1, 300 );
	if (opponent.ItemSlotArmor->Proto->ProtoId==585)
	{
		skill+=40;
		if (weight>6000)
			weight-=6000;
		else
			weight=0;
	}
	if (opponent.ItemSlotExt->Proto->ProtoId==210 || opponent.ItemSlotMain->Proto->ProtoId==210)
		skill+=100;
	if (opponent.Params[PE_GHOST] == 1 && GetNightPersonBonus() == 1 ) 
		skill+=30;
	if (opponent.Params[PE_IMPLANT_SCOUT] == 1)
		skill+=20;
	skill-=FOnline->LookSneakDir[lookdir]+weight/FOnline->LookWeight; //Nerf sneak by dir and by weight	
	sneakdist=fullpe - skill / 6; //Sneak distance formula
	sneakdist=CLAMP( sneakdist, 3, 300 ); //min sneak dist 3 hex
	return sneakdist;
}

int GetDirLook(float dir,int crDir)
{
	int lookdir=0;
	if (crDir==0)
	{
		if (( 0.0f   <= dir ) && ( dir < 60.0f  ))     lookdir=0;
		else if (( 60.0f  <= dir ) && ( dir < 120.0f ))lookdir=1;
		else if (( 120.0f <= dir ) && ( dir < 180.0f ))lookdir=2;
		else if (( 180.0f <= dir ) && ( dir < 240.0f ))lookdir=3;
		else if (( 240.0f <= dir ) && ( dir < 300.0f ))lookdir=2;
		else if (( 300.0f <= dir ) && ( dir < 360.0f ))lookdir=1;
	}
	else if (crDir==1)
	{
		if (( 0.0f   <= dir ) && ( dir < 60.0f  ))     lookdir=1;
		else if (( 60.0f  <= dir ) && ( dir < 120.0f ))lookdir=2;
		else if (( 120.0f <= dir ) && ( dir < 180.0f ))lookdir=3;
		else if (( 180.0f <= dir ) && ( dir < 240.0f ))lookdir=2;
		else if (( 240.0f <= dir ) && ( dir < 300.0f ))lookdir=1;
		else if (( 300.0f <= dir ) && ( dir < 360.0f ))lookdir=0;
	}
	else if (crDir==2)
	{
		if (( 0.0f   <= dir ) && ( dir < 60.0f  ))     lookdir=2;
		else if (( 60.0f  <= dir ) && ( dir < 120.0f ))lookdir=3;
		else if (( 120.0f <= dir ) && ( dir < 180.0f ))lookdir=2;
		else if (( 180.0f <= dir ) && ( dir < 240.0f ))lookdir=1;
		else if (( 240.0f <= dir ) && ( dir < 300.0f ))lookdir=0;
		else if (( 300.0f <= dir ) && ( dir < 360.0f ))lookdir=1;
	}
	else if (crDir==3)
	{
		if (( 0.0f   <= dir ) && ( dir < 60.0f  ))     lookdir=3;
		else if (( 60.0f  <= dir ) && ( dir < 120.0f ))lookdir=2;
		else if (( 120.0f <= dir ) && ( dir < 180.0f ))lookdir=1;
		else if (( 180.0f <= dir ) && ( dir < 240.0f ))lookdir=0;
		else if (( 240.0f <= dir ) && ( dir < 300.0f ))lookdir=1;
		else if (( 300.0f <= dir ) && ( dir < 360.0f ))lookdir=2;
	}
	else if (crDir==4)
	{
		if (( 0.0f   <= dir ) && ( dir < 60.0f  ))     lookdir=2;
		else if (( 60.0f  <= dir ) && ( dir < 120.0f ))lookdir=1;
		else if (( 120.0f <= dir ) && ( dir < 180.0f ))lookdir=0;
		else if (( 180.0f <= dir ) && ( dir < 240.0f ))lookdir=1;
		else if (( 240.0f <= dir ) && ( dir < 300.0f ))lookdir=2;
		else if (( 300.0f <= dir ) && ( dir < 360.0f ))lookdir=3;
	}
	else if (crDir==5)
	{
		if (( 0.0f   <= dir ) && ( dir < 60.0f  ))     lookdir=1;
		else if (( 60.0f  <= dir ) && ( dir < 120.0f ))lookdir=0;
		else if (( 120.0f <= dir ) && ( dir < 180.0f ))lookdir=1;
		else if (( 180.0f <= dir ) && ( dir < 240.0f ))lookdir=2;
		else if (( 240.0f <= dir ) && ( dir < 300.0f ))lookdir=3;
		else if (( 300.0f <= dir ) && ( dir < 360.0f ))lookdir=2;
	}
	return lookdir;
}

bool TraceWall(uint16 hx, uint16 hy, uint16 tx, uint16 ty, Map& map, int dist)
{

    float dir = GetDirectionF( hx, hy, tx, ty );
    uint8 dir1, dir2;
    if( ( 30.0f  <= dir ) && ( dir <  90.0f ) )     {dir1 = 5;dir2 = 0;}
    else if( ( 90.0f  <= dir ) && ( dir < 150.0f ) ){dir1 = 4;dir2 = 5;}
    else if( ( 150.0f <= dir ) && ( dir < 210.0f ) ){dir1 = 3;dir2 = 4;}
    else if( ( 210.0f <= dir ) && ( dir < 270.0f ) ){dir1 = 2;dir2 = 3;}
    else if( ( 270.0f <= dir ) && ( dir < 330.0f ) ){dir1 = 1;dir2 = 2;}
    else {dir1 = 0;dir2 = 1;}
    uint16 cx = hx;
    uint16 cy = hy;
    uint16 px = hx;
    uint16 py = hy;
    uint16 t1x, t1y, t2x, t2y;
    float  x1 = 3 * float(hx) + BIAS_FLOAT;
    float  y1 = SQRT3T2_FLOAT * float(hy) - ( float(hx % 2) ) * SQRT3_FLOAT + BIAS_FLOAT;
    float  x2 = 3 * float(tx) + BIAS_FLOAT;
    float  y2 = SQRT3T2_FLOAT * float(ty) - ( float(tx % 2) ) * SQRT3_FLOAT + BIAS_FLOAT;
    float  dx = x2 - x1;
    float  dy = y2 - y1;
    float  c1x, c1y, c2x, c2y; // test hex
    float  dist1, dist2;
    for( int i = 1; i < dist; i++ )
    {
        t1x = cx;
        t2x = cx;
        t1y = cy;
        t2y = cy;
		if (dir1==0 && ((cx % 2) == 0)){t1x-=1;}
		else if (dir1==0 && ((cx % 2) != 0)){t1x-=1;t1y-=1;}
		else if (dir1==1 && ((cx % 2) == 0)){t1x-=1;t1y+=1;}
		else if (dir1==1 && ((cx % 2) != 0)){t1x-=1;}
		else if (dir1==2)                   {t1y+=1;}
		else if (dir1==3 && ((cx % 2) == 0)){t1x+=1;t1y+=1;}
		else if (dir1==3 && ((cx % 2) != 0)){t1x+=1;}
		else if (dir1==4 && ((cx % 2) == 0)){t1x+=1;}
		else if (dir1==4 && ((cx % 2) != 0)){t1x+=1;t1y-=1;}
		else if (dir1==5)                   {t1y-=1;}
		
		if (dir2==0 && ((cx % 2) == 0)){t2x-=1;}
		else if (dir2==0 && ((cx % 2) != 0)){t2x-=1;t2y-=1;}
		else if (dir2==1 && ((cx % 2) == 0)){t2x-=1;t2y+=1;}
		else if (dir2==1 && ((cx % 2) != 0)){t2x-=1;}
		else if (dir2==2)                   {t2y+=1;}
		else if (dir2==3 && ((cx % 2) == 0)){t2x+=1;t2y+=1;}
		else if (dir2==3 && ((cx % 2) != 0)){t2x+=1;}
		else if (dir2==4 && ((cx % 2) == 0)){t2x+=1;}
		else if (dir2==4 && ((cx % 2) != 0)){t2x+=1;t2y-=1;}
		else if (dir2==5)                   {t2y-=1;}
	    c1x = 3 * float(t1x);
        c1y = SQRT3T2_FLOAT * float(t1y) - ( float(t1x % 2) ) * SQRT3_FLOAT;
        c2x = 3 * float(t2x);
        c2y = SQRT3T2_FLOAT * float(t2y) - ( float(t2x % 2) ) * SQRT3_FLOAT;
        dist1 = dx * ( y1 - c1y ) - dy * ( x1 - c1x );
        dist2 = dx * ( y1 - c2y ) - dy * ( x1 - c2x );
        dist1 = ( ( dist1 > 0 ) ? dist1 : -dist1 );
        dist2 = ( ( dist2 > 0 ) ? dist2 : -dist2 );
        if( dist1 <= dist2 )
        {
            cx = t1x;
            cy = t1y;
        }                                               // left hand biased
        else
        {
            cx = t2x;
            cy = t2y;
        }
        if( ( cx == px ) && ( cy == py ) )
            return false;
        else
        {
            px = cx;
            py = cy;
        }

        if(!map.IsHexRaked(cx, cy ) )return true;
	}
    return false;
} 


float GetDirectionF( uint16 hx, uint16 hy, uint16 tx, uint16 ty )
{
    float nx = 3 * ( float(tx) - float(hx) );
    float ny = SQRT3T2_FLOAT * ( float(ty) - float(hy) ) - ( float(tx % 2) - float(hx % 2) ) * SQRT3_FLOAT;
    return 180.0f + RAD2DEG* atan2( ny, nx ); // in degrees, because cvet loves the degrees
}

EXPORT bool check_trap_look(Map& map, Critter& cr, Item& trap)
{
	if (cr.CritterIsNpc)
		return true;
	if (cr.Params[VISION_LEVEL]>1)
		return true;
	int dist=GetDistantion(cr.HexX,cr.HexY,trap.AccHex.HexX,trap.AccHex.HexY);
	if (dist>9)
		return false;
	int perception=CLAMP(cr.Params[ DAMAGE_EYE ] ? 1 :cr.Params[ST_PERCEPTION]+cr.Params[ST_PERCEPTION_EXT],1,10);
	if (dist>perception/2 + CLAMP(cr.Params[SK_TRAPS]/50,0,4))
		return false;
	if (TraceWall(cr.HexX, cr.HexY, trap.AccHex.HexX, trap.AccHex.HexY, map, dist ))
		return false;
	return true;
}

EXPORT void Critter_GetLexems( Critter& cr, ScriptString& output )
{
	output.assign(cr.Lexems);
	return;
}

EXPORT void Item_GetLexems( Item& it, ScriptString& output )
{
	if(!it.Lexems)
	{
		output.assign("");
		return;
	}
	output.assign(it.Lexems);
	return;
}

EXPORT uint Critter_GetIp( Critter& cr )
{
	if( cr.CritterIsNpc )
		return( 0 );
	else
	{
		uint idx = cr.DataExt->CurrentIp;
		if( idx >= MAX_STORED_IP )
			return( 0 );
		else
			return( cr.DataExt->PlayIp[idx] );
	}
}

EXPORT float WmSpeed( Critter& cr )
{
    return( cr.GroupSelf->Speed );
}

EXPORT uint EncounterDescriptor( Critter& cr )
{
    return( cr.GroupSelf->EncounterDescriptor);
}

EXPORT bool IsCanBeMetOnWm( Critter& cr )
{
    return (cr.GroupSelf->CarId==0 && cr.GroupSelf->EncounterDescriptor==0 && cr.GroupSelf->Speed>0);
}

EXPORT void ReloadFunction(ScriptString& moduleName, ScriptString& funcdecl, ScriptString& code)
{
    asIScriptModule *execMod = ASEngine->GetModule(moduleName.c_str(), asGM_CREATE_IF_NOT_EXISTS);
    
    if (execMod)
    {
        Log( "Module loading okay.\n" );
    }
    else
    {
        Log( "Unable to found or create module.\n" );
        return;
    }
    asIScriptFunction *func = execMod->GetFunctionByDecl(funcdecl.c_str());

    if( func->GetId() > 0 )
    {
        execMod->RemoveFunction(func);
        Log("function found and removed\n");
    }
    else
    {
        Log("function not found\n");
    }
    execMod->CompileFunction(moduleName.c_str(),code.c_str(), 0, asCOMP_ADD_TO_MODULE, &func);
}

// sockaddr_in
#ifdef FO_LINUX
 #include <netinet/in.h>
#endif

#define UID_SIZE 5

struct ClientEx : Client
{
	uint UID[UID_SIZE];
	volatile int Sock; // in fact, SOCKET
	sockaddr_in From;
};

EXPORT uint Critter_GetUID(Critter& cr, uint8 num)
{
    if (cr.CritterIsNpc) return 0;
    if (num >= UID_SIZE) return 0;
    return ((ClientEx&)cr).UID[num];
}

EXPORT uint NativeStrToUint(ScriptString& str)
{
#if defined ( FO_WINDOWS )
    return uint(_atoi64(str.c_str()));
#elif defined ( FO_LINUX )
    return uint(atoll(str.c_str()));
#endif
}
#endif
