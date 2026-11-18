#include "MessageManager.h"

#include <map>
#include <set>
#include <string>

#include "EnumHelper.h"
#include "LuaManager.h"
#include "RageLog.h"
#include "RageThreads.h"
#include "global.h"

MessageManager* MESSAGEMAN =
    nullptr;  // global and accessible from anywhere in our program

static const char* MessageIDNames[] = {
    "CurrentGameChanged",
    "CurrentStyleChanged",
    "PlayModeChanged",
    "CoinsChanged",
    "CurrentSongChanged",
    "CurrentStepsP1Changed",
    "CurrentStepsP2Changed",
#if ITG_MAX_PLAYERS >= 4
    "CurrentStepsP3Changed",
    "CurrentStepsP4Changed",
#endif
    "CurrentCourseChanged",
    "CurrentTrailP1Changed",
    "CurrentTrailP2Changed",
#if ITG_MAX_PLAYERS >= 4
    "CurrentTrailP3Changed",
    "CurrentTrailP4Changed",
#endif
    "GameplayLeadInChanged",
    "EditStepsTypeChanged",
    "EditCourseDifficultyChanged",
    "EditSourceStepsChanged",
    "EditSourceStepsTypeChanged",
    "PreferredStepsTypeChanged",
    "PreferredDifficultyP1Changed",
    "PreferredDifficultyP2Changed",
#if ITG_MAX_PLAYERS >= 4
    "PreferredDifficultyP3Changed",
    "PreferredDifficultyP4Changed",
#endif
    "PreferredCourseDifficultyP1Changed",
    "PreferredCourseDifficultyP2Changed",
#if ITG_MAX_PLAYERS >= 4
    "PreferredCourseDifficultyP3Changed",
    "PreferredCourseDifficultyP4Changed",
#endif
    "EditCourseEntryIndexChanged",
    "EditLocalProfileIDChanged",
    "GoalCompleteP1",
    "GoalCompleteP2",
#if ITG_MAX_PLAYERS >= 4
    "GoalCompleteP3",
    "GoalCompleteP4",
#endif
    "NoteCrossed",
    "NoteWillCrossIn400Ms",
    "NoteWillCrossIn800Ms",
    "NoteWillCrossIn1200Ms",
    "CardRemovedP1",
    "CardRemovedP2",
#if ITG_MAX_PLAYERS >= 4
    "CardRemovedP3",
    "CardRemovedP4",
#endif
    "BeatCrossed",
    "MenuUpP1",
    "MenuUpP2",
#if ITG_MAX_PLAYERS >= 4
    "MenuUpP3",
    "MenuUpP4",
#endif
    "MenuDownP1",
    "MenuDownP2",
#if ITG_MAX_PLAYERS >= 4
    "MenuDownP3",
    "MenuDownP4",
#endif
    "MenuLeftP1",
    "MenuLeftP2",
#if ITG_MAX_PLAYERS >= 4
    "MenuLeftP3",
    "MenuLeftP4",
#endif
    "MenuRightP1",
    "MenuRightP2",
#if ITG_MAX_PLAYERS >= 4
    "MenuRightP3",
    "MenuRightP4",
#endif
    "MenuStartP1",
    "MenuStartP2",
#if ITG_MAX_PLAYERS >= 4
    "MenuStartP3",
    "MenuStartP4",
#endif
    "MenuSelectionChanged",
    "PlayerJoined",
    "PlayerUnjoined",
    "PlayerProfileSet",
    "AutosyncChanged",
    "PreferredSongGroupChanged",
    "PreferredCourseGroupChanged",
    "SortOrderChanged",
    "LessonTry1",
    "LessonTry2",
    "LessonTry3",
    "LessonCleared",
    "LessonFailed",
    "StorageDevicesChanged",
    "AutoJoyMappingApplied",
    "ScreenChanged",
    "SongModified",
    "ScoreMultiplierChangedP1",
    "ScoreMultiplierChangedP2",
#if ITG_MAX_PLAYERS >= 4
    "ScoreMultiplierChangedP3",
    "ScoreMultiplierChangedP4",
#endif
    "StarPowerChangedP1",
    "StarPowerChangedP2",
#if ITG_MAX_PLAYERS >= 4
    "StarPowerChangedP3",
    "StarPowerChangedP4",
#endif
    "CurrentComboChangedP1",
    "CurrentComboChangedP2",
#if ITG_MAX_PLAYERS >= 4
    "CurrentComboChangedP3",
    "CurrentComboChangedP4",
#endif
    "StarMeterChangedP1",
    "StarMeterChangedP2",
#if ITG_MAX_PLAYERS >= 4
    "StarMeterChangedP3",
    "StarMeterChangedP4",
#endif
    "LifeMeterChangedP1",
    "LifeMeterChangedP2",
#if ITG_MAX_PLAYERS >= 4
    "LifeMeterChangedP3",
    "LifeMeterChangedP4",
#endif
    "UpdateScreenHeader",
    "LeftClick",
    "RightClick",
    "MiddleClick",
    "MouseWheelUp",
    "MouseWheelDown",
};
XToString(MessageID);

static RageMutex g_Mutex("MessageManager");

typedef std::set<IMessageSubscriber*> SubscribersSet;
static std::map<std::string, SubscribersSet> g_MessageToSubscribers;

Message::Message(const std::string& s) {
  m_sName = s;
  m_pParams = new LuaTable;
  m_bBroadcast = false;
}

Message::Message(const MessageID id) {
  m_sName = MessageIDToString(id);
  m_pParams = new LuaTable;
  m_bBroadcast = false;
}

Message::Message(const std::string& s, const LuaReference& params) {
  m_sName = s;
  m_bBroadcast = false;
  Lua* L = LUA->Get();
  m_pParams = new LuaTable;  // XXX: creates an extra table
  params.PushSelf(L);
  m_pParams->SetFromStack(L);
  LUA->Release(L);
  //	m_pParams = new LuaTable( params );
}

Message::~Message() { delete m_pParams; }

void Message::PushParamTable(lua_State* L) { m_pParams->PushSelf(L); }

void Message::SetParamTable(const LuaReference& params) {
  Lua* L = LUA->Get();
  params.PushSelf(L);
  m_pParams->SetFromStack(L);
  LUA->Release(L);
}

const LuaReference& Message::GetParamTable() const { return *m_pParams; }

void Message::GetParamFromStack(lua_State* L, const std::string& sName) const {
  m_pParams->Get(L, sName);
}

void Message::SetParamFromStack(lua_State* L, const std::string& sName) {
  m_pParams->Set(L, sName);
}

MessageManager::MessageManager() {
  m_Logging = false;
  // Register with Lua.
  {
    Lua* L = LUA->Get();
    lua_pushstring(L, "MESSAGEMAN");
    this->PushSelf(L);
    lua_settable(L, LUA_GLOBALSINDEX);
    LUA->Release(L);
  }
}

MessageManager::~MessageManager() {
  // Unregister with Lua.
  LUA->UnsetGlobal("MESSAGEMAN");
}

void MessageManager::Subscribe(
    IMessageSubscriber* pSubscriber, const std::string& sMessage) {
  LockMut(g_Mutex);

  SubscribersSet& subs = g_MessageToSubscribers[sMessage];
#ifdef DEBUG
  SubscribersSet::iterator iter = subs.find(pSubscriber);
  ASSERT_M(
      iter == subs.end(),
      ssprintf("already subscribed to '%s'", sMessage.c_str()));
#endif
  subs.insert(pSubscriber);
}

void MessageManager::Subscribe(IMessageSubscriber* pSubscriber, MessageID m) {
  Subscribe(pSubscriber, MessageIDToString(m));
}

void MessageManager::Unsubscribe(
    IMessageSubscriber* pSubscriber, const std::string& sMessage) {
  LockMut(g_Mutex);

  SubscribersSet& subs = g_MessageToSubscribers[sMessage];
  SubscribersSet::iterator iter = subs.find(pSubscriber);
  ASSERT(iter != subs.end());
  subs.erase(iter);
}

void MessageManager::Unsubscribe(IMessageSubscriber* pSubscriber, MessageID m) {
  Unsubscribe(pSubscriber, MessageIDToString(m));
}

void MessageManager::Broadcast(Message& msg) const {
  if (m_Logging) {
    LOG->Trace("MESSAGEMAN:Broadcast: %s", msg.GetName().c_str());
  }
  msg.SetBroadcast(true);

  LockMut(g_Mutex);

  std::map<std::string, SubscribersSet>::const_iterator iter =
      g_MessageToSubscribers.find(msg.GetName());
  if (iter == g_MessageToSubscribers.end()) {
    return;
  }

  for (IMessageSubscriber* subscriber : iter->second) {
    subscriber->HandleMessage(msg);
  }
}

void MessageManager::Broadcast(const std::string& sMessage) const {
  ASSERT(!sMessage.empty());
  Message msg(sMessage);
  Broadcast(msg);
}

void MessageManager::Broadcast(MessageID m) const {
  Broadcast(MessageIDToString(m));
}

bool MessageManager::IsSubscribedToMessage(
    IMessageSubscriber* pSubscriber, const std::string& sMessage) const {
  SubscribersSet& subs = g_MessageToSubscribers[sMessage];
  return subs.find(pSubscriber) != subs.end();
}

void IMessageSubscriber::ClearMessages(const std::string sMessage) {}

MessageSubscriber::MessageSubscriber(const MessageSubscriber& cpy)
    : IMessageSubscriber(cpy) {
  for (const std::string& msg : cpy.m_vsSubscribedTo) {
    this->SubscribeToMessage(msg);
  }
}

MessageSubscriber& MessageSubscriber::operator=(const MessageSubscriber& cpy) {
  if (&cpy == this) {
    return *this;
  }

  UnsubscribeAll();

  for (const std::string& msg : cpy.m_vsSubscribedTo) {
    this->SubscribeToMessage(msg);
  }

  return *this;
}

void MessageSubscriber::SubscribeToMessage(const std::string& sMessageName) {
  MESSAGEMAN->Subscribe(this, sMessageName);
  m_vsSubscribedTo.push_back(sMessageName);
}

void MessageSubscriber::SubscribeToMessage(MessageID message) {
  MESSAGEMAN->Subscribe(this, message);
  m_vsSubscribedTo.push_back(MessageIDToString(message));
}

void MessageSubscriber::UnsubscribeAll() {
  for (const std::string& s : m_vsSubscribedTo) {
    MESSAGEMAN->Unsubscribe(this, s);
  }
  m_vsSubscribedTo.clear();
}

// lua start
#include "LuaBinding.h"

/** @brief Allow Lua to have access to the MessageManager. */
class LunaMessageManager : public Luna<MessageManager> {
 public:
  static int Broadcast(T* p, lua_State* L) {
    if (!lua_istable(L, 2) && !lua_isnoneornil(L, 2)) {
      luaL_typerror(L, 2, "table or nil");
    }

    LuaReference ParamTable;
    lua_pushvalue(L, 2);
    ParamTable.SetFromStack(L);

    Message msg(SArg(1), ParamTable);
    p->Broadcast(msg);
    COMMON_RETURN_SELF;
  }
  static int SetLogging(T* p, lua_State* L) {
    p->SetLogging(lua_toboolean(L, -1));
    COMMON_RETURN_SELF;
  }

  LunaMessageManager() {
    ADD_METHOD(Broadcast);
    ADD_METHOD(SetLogging);
  }
};

LUA_REGISTER_CLASS(MessageManager)
// lua end

/*
 * (c) 2003-2004 Chris Danford
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
