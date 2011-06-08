#include "Command.h"


namespace com{
//============================================================================================================
//	Klasse Command:
//  Schnitstelle im sinne des Command Pattern.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Command::Ptr Command::createReverseCommand() {
	return Command::Ptr( new CmdNull() ); 
}
//============================================================================================================
//	Klasse CommandExecuter:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CommandExecuter::executeCommands() { 
	while ( true ) {
		Command::Ptr cmd;
		{	// namensraum fuer mutex ( autom. unlock )
			boost::lock_guard<boost::timed_mutex> lock( CommandExecuter::mutex );
			Command::Container::iterator it = commands.begin();
			if ( it == commands.end() ) return;
			
			cmd = *it;  
			commands.erase ( it );
		}   // namensraum fuer mutex ( auom. unlock )
		if ( cmd ) cmd->execute();
	}
}
//============================================================================================================
//	Klasse CommandWorker:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
CommandWorker::Ptr CommandWorker::singleton;
//------------------------------------------------------------------------------------------------------------
CommandWorker::Ptr CommandWorker::getCommandWorker() {
	if (singleton) return singleton;
	singleton = Ptr( new CommandWorker( Settings::COMMAND_WORKER_INTERVAL_MS ) );
	singleton->start();
	return singleton;
}
//------------------------------------------------------------------------------------------------------------
CommandWorker::~CommandWorker() {
}
//------------------------------------------------------------------------------------------------------------
void CommandWorker::callBack() {
	// mutex doppelt gemopplet da exec. nebenl. aufuehrbar
	//boost::unique_lock<boost::timed_mutex> lock( _mutex, boost::try_to_lock);
	//if (!lock.owns_lock() ) return;
	executeCommands();
}
} // namespace com
