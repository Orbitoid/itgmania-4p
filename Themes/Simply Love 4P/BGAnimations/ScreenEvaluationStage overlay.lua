-- 4P: compact four-column evaluation for the quad style; stock layout otherwise.
if not IsQuadStyle() then
	return Def.ActorFrame{ LoadActor("/Themes/Simply Love/BGAnimations/ScreenEvaluation common/default.lua") }
end

local song = GAMESTATE:GetCurrentSong()
local af = Def.ActorFrame{ Name="QuadEval" }

af[#af+1] = Def.BitmapText{
	Font="Common Normal",
	InitCommand=function(self)
		self:xy(_screen.cx, 44):zoom(1.1):settext(song and song:GetDisplayFullTitle() or "")
	end,
}

local rows = {
	{ "TapNoteScore_W1", "Fantastic", color("#21CCE8") },
	{ "TapNoteScore_W2", "Excellent", color("#E29C18") },
	{ "TapNoteScore_W3", "Great",     color("#66C955") },
	{ "TapNoteScore_W4", "Decent",    color("#B45CFF") },
	{ "TapNoteScore_W5", "Way Off",   color("#C9855E") },
	{ "TapNoteScore_Miss", "Miss",    color("#FF3030") },
}

for i, player in ipairs(PlayerNumber) do
	if GAMESTATE:IsHumanPlayer(player) then
		local pss = STATSMAN:GetCurStageStats():GetPlayerStageStats(player)
		local col = PlayerColor(player)
		local cx = Get4PQuarterX(player)
		local steps = GAMESTATE:GetCurrentSteps(player)
		local pf = Def.ActorFrame{ Name="Eval"..ToEnumShortString(player), InitCommand=function(self) self:x(cx) end }
		local w = 190
		pf[#pf+1] = Def.Quad{ InitCommand=function(self) self:xy(0, 250):zoomto(w+4, 330):diffuse(col) end }
		pf[#pf+1] = Def.Quad{ InitCommand=function(self) self:xy(0, 250):zoomto(w, 326):diffuse(0.04,0.04,0.07,0.95) end }
		pf[#pf+1] = Def.BitmapText{ Font="Common Bold", InitCommand=function(self)
			self:xy(0, 110):zoom(0.45):diffuse(col):settext("PLAYER "..i) end }
		if steps then
			pf[#pf+1] = Def.BitmapText{ Font="Common Normal", InitCommand=function(self)
				self:xy(0, 136):zoom(0.9):diffuse(DifficultyColor(steps:GetDifficulty()))
				  :settext(THEME:GetString("Difficulty", ToEnumShortString(steps:GetDifficulty())) .. "  " .. steps:GetMeter())
			end }
		end
		pf[#pf+1] = Def.BitmapText{ Font="Wendy/_wendy white", InitCommand=function(self)
			self:xy(0, 176):zoom(0.55):diffuse(col):settext(string.format("%.2f%%", pss:GetPercentDancePoints() * 100)) end }
		local failed = pss:GetFailed()
		pf[#pf+1] = Def.BitmapText{ Font="Common Normal", InitCommand=function(self)
			self:xy(0, 208):zoom(0.9):settext(failed and "FAILED" or "CLEARED"):diffuse(failed and color("#FF3030") or color("#4CFF5C")) end }
		for r, row in ipairs(rows) do
			local y = 240 + (r - 1) * 26
			pf[#pf+1] = Def.BitmapText{ Font="Common Normal", InitCommand=function(self)
				self:xy(-w/2 + 10, y):halign(0):zoom(0.9):diffuse(row[3]):settext(row[2]) end }
			pf[#pf+1] = Def.BitmapText{ Font="Common Normal", InitCommand=function(self)
				self:xy(w/2 - 10, y):halign(1):zoom(0.9):settext(pss:GetTapNoteScores(row[1])) end }
		end
		pf[#pf+1] = Def.BitmapText{ Font="Common Normal", InitCommand=function(self)
			self:xy(-w/2 + 10, 240 + 6*26 + 8):halign(0):zoom(0.9):settext("Max Combo") end }
		pf[#pf+1] = Def.BitmapText{ Font="Common Normal", InitCommand=function(self)
			self:xy(w/2 - 10, 240 + 6*26 + 8):halign(1):zoom(0.9):settext(pss:MaxCombo()) end }
		af[#af+1] = pf
	end
end
return af
