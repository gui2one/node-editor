# TODOs
 - Make it the best thing EVER !
 - UNDO/REDO (almost done, still missing network view changes )
 - Better network discovery/update. For now, the node network is evaluated very naïvely. Every node connected to NodeManager m_OutputNode are "recooked", no matter if their content changed or not. It would be nice to have a "CookState" or something, and make sure the node need to be recooked or it can use a cache. 
 I could use the already generated m_ParamsHash for that.
 - Param ColorWithAlpha
 - Serialize node color and make it changeable

## ImGui
- flexible modal dialog 

## Bugs to fix
 - ImGui drawing issue after window resize. ( this doesn't happen anymore, I have no idea why ...)
 - AbstractNode::m_ParamsHash hash for subnet node ( <b>because it uses AbstractNode::YAMLSerialize() method</b> ) takes into account child nodes positions, <b> which is bad</b>. Find a way to <b>recursively</b> ignore child nodes positions like I do for a normal node