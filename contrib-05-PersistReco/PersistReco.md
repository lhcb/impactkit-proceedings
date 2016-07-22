The PersistReco flag available in Turbo stream HLT2 lines allows all the reconstructed objects in an event to be saved to disk, rather than the usual behaviour of saving only the selected candidate. The objects can then be used in DaVinci just like in the stripping. This is useful when one wants to do things like spectroscopy studies, where there may be many final states of interesting that re-use the same ground state particle. Today, there is a bug that can cause the same underlying particle object to be used multiple times when building a decay. For example, if one takes a D0 → K-π+ candidate that fired an HLT2 line, and adds a pion from PersistReco, it is possible to end up with D0 π candidates where the pion from the D0 is also used as the bachelor. This behaviour should not occur, as it is not physical and is not useful for analysis, and can even give spurious physics results.


The function CombineParticles()::checkOverlap() normally ensures this does not happen. The code "Phys/DaVinciOverlapsAndClones/src/CheckOverlap.cpp", which contains this function has been modified, so that no more duplicates will be used even in the case of objects from different locations.

```C++
//===========================================================================
// Check duplicate entries
//===========================================================================
bool CheckOverlap::searchOverlap( std::vector<const LHCb::ProtoParticle* > & proto )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "searchOverlap(protos)" << endmsg ;
  // It its a simple particle made from protoparticle. Check.

  for (std::vector<const LHCb::ProtoParticle* >::const_iterator i = proto.begin();
       i != proto.end() ; ++i)
  {
    for (std::vector<const LHCb::ProtoParticle* >::const_iterator j = i ;
         j != proto.end(); ++j)
    {
      if (j==i) continue ;
      if ( *i==*j )
      {
          if (msgLevel(MSG::VERBOSE)) verbose() << "Found overlap " << *i << endmsg ;
          return true ;
      }
      else {
        const std::vector<LHCb::LHCbID> i_ids = (*i)->track()->lhcbIDs();
        const std::vector<LHCb::LHCbID> j_ids = (*j)->track()->lhcbIDs();
        std::vector<LHCb::LHCbID> diff;
        std::set_symmetric_difference(i_ids.begin(), i_ids.end(), j_ids.begin(), j_ids.end(), std::back_inserter(diff));

        if (std::all_of(diff.begin(), diff.end(), [] (LHCb::LHCbID id) { return id.isCalo(); } ))
        {
          if (msgLevel(MSG::VERBOSE)) verbose() << "Found overlap using LHCb IDs, ignoring isCalo IDs" << *i << endmsg ;
          return true;
        }
      }
     }
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "Found no overlap" << endmsg ;
  return false;
}
```
