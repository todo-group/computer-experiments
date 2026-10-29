-- Keep TeX intact for the directly loaded MathJax 4. Pandoc's MathML/plain
-- conversions do not support all lecture notation (aligned, textcolor, rm).
-- Raw HTML prevents the writer from attempting either conversion.
function Math(math)
  if FORMAT ~= "revealjs" then
    return nil
  end
  local tex = math.text:gsub("&", "&amp;"):gsub("<", "&lt;"):gsub(">", "&gt;")
  local display = math.mathtype == "DisplayMath"
  local class = display and "display" or "inline"
  local opening = display and "\\[" or "\\("
  local closing = display and "\\]" or "\\)"
  return pandoc.RawInline("html",
    '<span class="math ' .. class .. '">' .. opening .. tex .. closing .. '</span>')
end
