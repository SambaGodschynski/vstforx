#include "com/PluginCollection.h"
#include <iostream>
#include "audioeffectx.h"
#include "com/one4All.h"
#include "processing/PlugNode.h"
using namespace std;
using namespace com;
using namespace events;

audioMasterCallback hostCallback = NULL;

#define KEY_IN { char c[255]; scanf(&c[0]); }//cin>>c; }

//-----------------------------------------------------------------------------
struct OnLoadFileListener : public EventListener<OnLoadFile> {
	void eventHandler ( void *src, const OnLoadFile &ev ) {
		cout<<ev.filename<<endl;
	}
};
//-----------------------------------------------------------------------------
void testIsSubDirectory() {
	cout<<"Teste isSubDirectory methode:"<<endl;
	string p[] = { "c:\\spiele", "c:\\spiele\\" };
	string s[] = { "c:\\spiele\\boxen", "c:\\spiele\\", "c:/spiele/gta", "d:/spiele/",
		"d:/", "c:\\", "c:/Spiele/boxen/maps" };
	for ( int i=0; i<sizeof(s)/sizeof(s[0]); ++i ) {
		bool b;
		for ( int j=0; j<sizeof(p)/sizeof(p[0]); ++j ) {
			b = isSubDirectory ( p[j], s[i] );
			cout<<p[j]<<" <=> "<<s[i]<<" : "<<( b ? "yes" : "no" )<<endl;
		}
		cout<<endl;
	}
}
//-----------------------------------------------------------------------------
void scanForPlugs() {
	typedef processing::PluginInfo ProxyList;
	OnLoadFileListener l;
	PluginCollection::Ptr plugC = PluginCollection::getPluginCollection( &l );
	size_t loadedF = plugC->getPlugTree().size(); 
	cout<<"............................................................................."<<endl;
	cout<<loadedF<<" vst-plugins found.";
	cout<<" Accessable("<<plugC->getInfo().numAccessablePlugs<<"), ";
	cout<<"inaccessible("<<plugC->getInfo().numInaccessablePlugs<<")."<<endl;
	KEY_IN;
}
//-----------------------------------------------------------------------------
int main ( int argc, char **argv ){
	scanForPlugs();
	cout<<"press key to exit."<<endl;
	KEY_IN;
	return 0;
}
//---------------------------------------------------------------------------------------
const std::string  getHomeDirectory() {
	const size_t N = 512; 
	char _d[N];
	DWORD r = GetModuleFileName ( NULL, &_d[0], N );
	com::Filename f( _d  );
	if ( is_regular_file(f) ) return f.remove_filename().string();
	return f.directory_string();
	
}
