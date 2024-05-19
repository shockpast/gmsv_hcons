if not SERVER then return end
if not system.IsLinux() then return end

require("hcons")

-- msgfilter searches for a certain input in messages;
-- therefore makes it much easier to ignore them
local messages = {
  "do_constraint_system",
  "Ignoring unreasonable",
  "CreateFragmentsFromFile"
}

for _, input in ipairs(messages) do
  hcons.msg_filter.ignore_message(input)
end
