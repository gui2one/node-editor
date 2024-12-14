# TODOs

- Make it the best thing EVER !
- UNDO/REDO (almost done, still missing network view changes, and node de/selection )
- Better network discovery/update. For now, the node network is evaluated very naïvely. Every node connected to NodeManager m_OutputNode are "recooked", no matter if their content changed or not. It would be nice to have a "CookState" or something, and make sure the node need to be recooked or it can use a cache. 
- Duplicate nodes ( almost done )
 Cloning a subnet node should clone it's content too ( recursively ??!!!! )

# Bugs to fix
 - bad connection for nodes inside a cloned subnet.