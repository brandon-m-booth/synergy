/*
 * libsynergy -- mouse and keyboard sharing utility
 * Copyright (C) 2012 Synergy Si Ltd.
 * Copyright (C) 2002 Chris Schoeneman
 * 
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 * 
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define WINAPI_CALLBACK (1) // Enables callbacks

#include "synergy/ClientApp.h"
#include "arch/Arch.h"
#include "base/Log.h"
#include "base/EventQueue.h"

#if WINAPI_XWINDOWS
#include "libsynergyc/XWindowsClientTaskBarReceiver.h"
#else
#error Platform not supported.
#endif

#include <boost/thread.hpp>

ClientApp* app;
std::string targetServer;
boost::thread* clientThread = NULL;

bool GetHasFakeMouseMoveEvents()
{
   return app->getClientXWindowsScreenPtr()->getHasFakeMouseMoveEvents();
}

bool GetHasFakeKeyStateEvents()
{
   return app->getClientXWindowsScreenPtr()->getHasFakeKeyStateEvents();
}

bool GetNextMouseMoveEvent(std::pair<int32_t, int32_t>& mousePos)
{
   PlatformScreen::MousePosition mousePosition;
   if (app->getClientXWindowsScreenPtr()->GetNextMouseMoveEvent(mousePosition))
   {
      mousePos.first = mousePosition.x;
      mousePos.second = mousePosition.y;
      return true;
   }
   return false;
}

bool GetNextKeyStateEvent(std::pair<unsigned int, bool>& keyState)
{
   PlatformScreen::KeyState ks;
   if (app->getClientXWindowsScreenPtr()->GetNextKeyStateEvent(ks))
   {
      keyState.first = ks.keyCode;
      keyState.second = ks.isDown;
      return true;
   }
   return false;
}

static void Run()
{
   int argc = 2;
   char foregroundOption[] = "-f";
   char* argv[argc];
   argv[0] = foregroundOption;
   argv[1] = const_cast<char*>(targetServer.c_str());
	app->run(argc, argv);
}

void Initialize(std::string serverName)
{
	Arch arch;
	arch.init();

	Log log;
	EventQueue events;

	app = new ClientApp(&events, createTaskBarReceiver);

   targetServer = serverName;
   clientThread = new boost::thread(&Run);
}

void Shutdown()
{
   clientThread->join();
   delete app;
}
