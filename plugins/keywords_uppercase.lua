--[[
Sample plugin file for highlight 3.9
]]

Description="Convert keywords to upper case, if the syntax is not case sensitive."

-- optional parameter: syntax description
function syntaxUpdate(desc)

  if IgnoreCase ~=true then
     return
  end

  function Decorate(token, state)
    if (state == HL_KEYWORD) then
      return  string.upper(token)
    end
  end

end

Plugins={

  { Type="lang", Chunk=syntaxUpdate },

}
