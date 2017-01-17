// ============================================================================
#ifndef LOKI_PARTICLES38_H 
#define LOKI_PARTICLES38_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles0.h"
#include "LoKi/VertexHolder.h"
#include "LoKi/AuxDesktopBase.h"
// ============================================================================
/** @file LoKi/Particles38.h
 *
 *  Collection of functions for 'corrected mass' variable by Mike Williams, 
 *  see <a href="http://indico.cern.ch/conferenceDisplay.py?confId=104542">
 *  Mike's slides at Trigger meeting 28 September 2k+10 </a> and 
 *  <a href="http://savannah.cern.ch/task/?17418">Savannah task #17418</a>
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @thanks Mike Williams
 *
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 *  @date   2010-10-23
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class PtFlight 
     *  Simple evaluator for transverse momentum relative to flight direction 
     *  @see LoKi::Cuts:PTFLIGHT 
     *  @see LoKi::Particles::TransverseMomentumRel  
     *  @thanks Mike Williams
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    // ========================================================================
    class GAUDI_API PtFlight
      : public  LoKi::Particles::TransverseMomentumRel  
      , public  LoKi::Vertices::VertexHolder    
    {
    public:
      // ======================================================================
      /** constructor from the primary vertex
       *  @param pv  the primary vertex 
       */
      PtFlight ( const LHCb::VertexBase* pv    ) ;
      /** constructor from the primary vertex
       *  @param point  the position of primary vertex 
       */
      PtFlight ( const LoKi::Point3D&    point ) ;
      /** constructor from the primary vertex
       *  @param x the x-position of primary vertex 
       *  @param y the x-position of primary vertex 
       *  @param z the x-position of primary vertex 
       */
      PtFlight ( const double x , 
                 const double y , 
                 const double z ) ;
      /// MANDATORY: clone method ("virtual constructor")
      PtFlight* clone() const override;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const override;
      /// OPTIONAL: the specific printout 
      std::ostream& fillStream( std::ostream& s ) const override;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      PtFlight ( ) ;
      // ======================================================================
    public:
      // ======================================================================
      /** evaluate the transverse momentum versus the flight direction 
       *  @param p  (INPUT) the 4-momentum
       *  @param sv (INPUT) the decay  vertex of the particle  
       *  @param pv (INPUT) the origin vertex of the particle  
       *  @return the transverse momentum versus the fligth direction
       *  @see LoKi::TransverseMomentumRel::ptDir 
       */
      double ptFlight 
      ( const LoKi::LorentzVector& p  , 
        const LoKi::Point3D&       sv , 
        const LoKi::Point3D&       pv ) const { return ptDir ( p , sv - pv ) ; }
      // ======================================================================
      /** evaluate the 'corrected' mass of the particle 
       *  \f[  \vec{i} = \sqrt{ M^2 + 
       *            \left|p_{T}^{\prime}\right|^2 } +
       *            \left|p_{T}^{\prime}\right|, \f] 
       *  where \f$ \left|p_{T}^{\prime}\right|\f$ stands for the 
       *  @param p  (INPUT) the 4-momentum
       *  @param sv (INPUT) the decay  vertex of the particle  
       *  @param pv (INPUT) the origin vertex of the particle  
       *  @return the corrected mass
       *  @see LoKi::PtFlight::ptDir 
       *  @see LoKi::TransverseMomentumRel::ptDir 
       *  @thanks Mike Williams
       */
      double mCorrFlight
      ( const LoKi::LorentzVector& p  , 
        const LoKi::Point3D&       sv , 
        const LoKi::Point3D&       pv ) const 
      { return mCorrDir ( p , sv - pv ) ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MCorrected
     *  Simple evaluator for 'corrected' mass relative to flight direction 
     *  \f[  \vec{i} = \sqrt{ M^2 + 
     *            \left|p_{T}^{\prime}\right|^2 } +
     *            \left|p_{T}^{\prime}\right|, \f] 
     *  where \f$ \left|p_{T}^{\prime}\right|\f$ stands for the 
     *  transverse momentum relative to the fligth direction 
     *  
     *  For more information see 
     *  <a href="http://indico.cern.ch/conferenceDisplay.py?confId=104542">
     *  Mike's slides at Trigger meeting 28 September 2k+10 </a>
     *
     *  @thanks Mike Williams
     *  @see LoKi::Cuts::CORRM
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    // ========================================================================
    class GAUDI_API MCorrected : public  LoKi::Particles::PtFlight  
    {
    public:
      // ======================================================================
      /** constructor from the primary vertex
       *  @param pv  the primary vertex 
       */
      MCorrected ( const LHCb::VertexBase* pv    ) ;
      /** constructor from the primary vertex
       *  @param point  the position of primary vertex 
       */
      MCorrected ( const LoKi::Point3D&    point ) ;
      /** constructor from the primary vertex
       *  @param x the x-position of primary vertex 
       *  @param y the x-position of primary vertex 
       *  @param z the x-position of primary vertex 
       */
      MCorrected ( const double x , 
                   const double y , 
                   const double z ) ;
      /// MANDATORY: clone method ("virtual constructor")
      MCorrected* clone() const override;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const override;
      /// OPTIONAL: the specific printout 
      std::ostream& fillStream( std::ostream& s ) const override;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      MCorrected ( ) ;
      // ======================================================================
    } ;


    // ========================================================================
    /** @class BremMCorrected
     *  Simple evaluator for 'HOP' mass 
     *  
     *  For more information see 
     *  <a href="https://cds.cern.ch/record/2102345/files/LHCb-INT-2015-037.pdf">
     *
     *  @authors Pavol Stefko pavol.stefko@epfl.ch, Guido Andreassi guido.andreassi@epfl.ch, Violaine Bellee violaine.bellee@epfl.ch
     *  @date   2017-01-17
     */
    // ========================================================================
    class GAUDI_API BremMCorrected : public  LoKi::Particles::PtFlight 
    {
    public:
      // ======================================================================
      /** constructor from the primary vertex
       *  @param pv  the primary vertex 
       */
      BremMCorrected ( const LHCb::VertexBase* pv    ) ;
      /** constructor from the primary vertex
       *  @param point  the position of primary vertex 
       */
      BremMCorrected ( const LoKi::Point3D&    point ) ;
      /** constructor from the primary vertex
       *  @param x the x-position of primary vertex 
       *  @param y the x-position of primary vertex 
       *  @param z the x-position of primary vertex 
       */
      BremMCorrected ( const double x , 
		       const double y , 
		       const double z ) ;
      // MANDATORY: clone method ("virtual constructor") 
      BremMCorrected* clone() const override;
      // MANDATORY: the only one essential method      
      result_type operator () ( argument p ) const override;
      // OPTIONAL: the specific printout
      std::ostream& fillStream( std::ostream& s ) const override;

    private:
      // ======================================================================
      /// default constructor is disabled 
      BremMCorrected ( ) ;
      // ======================================================================
      bool has_electron (const LHCb::Particle *parent) const;
      bool has_only_electrons (const LHCb::Particle *parent) const;
      bool classify (const LHCb::Particle *parent) const;
      
      std::vector <LHCb::Particle*> *m_all_electrons  = new std::vector<LHCb::Particle*>;
      std::vector <LHCb::Particle*> *m_rest_electrons  = new std::vector<LHCb::Particle*>;
      std::vector <LHCb::Particle*> *m_electron_mothers  = new std::vector<LHCb::Particle*>;
      std::vector <LHCb::Particle*> *m_others  = new std::vector<LHCb::Particle*>;

      static constexpr double m_e_PDG = 0.510998910;
    };

    // ========================================================================
    /** @class PtFlightWithBestVertex 
     *  Simple evaluator for transverse momentum relative to flight direction 
     *  @see LoKi::Cuts::BPVPTFLIGHT 
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     *  @thanks Mike Williams
     */
    // ========================================================================
    class GAUDI_API PtFlightWithBestVertex 
      : public          LoKi::Particles::PtFlight 
      , public virtual  LoKi::AuxDesktopBase 
    {
    public: 
      // =====================================================================
      /// constructor 
      PtFlightWithBestVertex();
      /// MANDATORY: clone method ("virtual constructor")
      PtFlightWithBestVertex* clone() const override;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const override;
      /// OPTIONAL: the specific printout 
      std::ostream& fillStream( std::ostream& s ) const override;
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class MCorrectedWithBestVertex  
     *  Simple evaluator for corrected mass relative to flight direction 
     *  \f[  \vec{i} = \sqrt{ M^2 + 
     *            \left|p_{T}^{\prime}\right|^2 } +
     *            \left|p_{T}^{\prime}\right|, \f] 
     *  where \f$ \left|p_{T}^{\prime}\right|\f$ stands for the 
     *  transverse momentum relative to the fligth direction 
     *
     *  For more information see 
     *  <a href="http://indico.cern.ch/conferenceDisplay.py?confId=104542">
     *  Mike's slides at Trigger meeting 28 September 2k+10 </a>
     *
     *  @see LoKi::Cuts::BPVCORRM 
     *  @see LoKi::Cuts::CORRM 
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     *  @thanks Mike Williams
     */
    // ========================================================================
    class GAUDI_API MCorrectedWithBestVertex : public PtFlightWithBestVertex 
    {
    public: 
      // =====================================================================
      /// constructor 
      MCorrectedWithBestVertex() = default;
      /// MANDATORY: clone method ("virtual constructor")
      MCorrectedWithBestVertex* clone() const override;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const override;
      /// OPTIONAL: the specific printout 
      std::ostream& fillStream( std::ostream& s ) const override;
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class BremMCorrectedWithBestVertex
     *  Simple evaluator for 'HOP' mass 
     *  
     *  For more information see 
     *  <a href="https://cds.cern.ch/record/2102345/files/LHCb-INT-2015-037.pdf">
     *
     *  @authors Pavol Stefko pavol.stefko@epfl.ch, Guido Andreassi guido.andreassi@epfl.ch, Violaine Bellee violaine.bellee@epfl.ch
     *  @date   2017-01-17
     */
    // ========================================================================
    class GAUDI_API BremMCorrectedWithBestVertex : public PtFlightWithBestVertex 
    {
    public: 
      // =====================================================================
      /// constructor 
      BremMCorrectedWithBestVertex() = default;
      /// MANDATORY: clone method ("virtual constructor")
      BremMCorrectedWithBestVertex* clone() const override;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const override;
      /// OPTIONAL: the specific printout 
      std::ostream& fillStream( std::ostream& s ) const override;
    private:      
      bool has_electron (const LHCb::Particle *parent) const;
      bool has_only_electrons (const LHCb::Particle *parent) const;
      bool classify (const LHCb::Particle *parent) const;

      
      std::vector <LHCb::Particle*> *m_all_electrons  = new std::vector<LHCb::Particle*>;
      std::vector <LHCb::Particle*> *m_rest_electrons  = new std::vector<LHCb::Particle*>;
      std::vector <LHCb::Particle*> *m_electron_mothers  = new std::vector<LHCb::Particle*>;
      std::vector <LHCb::Particle*> *m_others  = new std::vector<LHCb::Particle*>;

      static constexpr double m_e_PDG = 0.510998910;
      // ======================================================================
    } ;  
    // ========================================================================
  }  //                                        end of namespace LoKi::Particles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================    
    /** @typedef PTFLIGHT 
     *  Simple functor to evaluate the transverse momentum with respect to 
     *  particle fligth direction
     * 
     *  @code 
     * 
     *   // get the primary vertex: 
     *   const LHCb::VertexBase* primary = ... ; 
     * 
     *   // create the functor 
     *   const PTFLIGHT fun = PTFLIGHT ( primary ) ;
     * 
     *   const LHCb::Particle* B = ... ;
     *
     *   // use functor:
     *   const double ptFlight = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::PtFlight 
     *  @see LoKi::Particles::TransverseMomentumRel 
     *  @see LoKi::Cuts::PTDIR
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     *  @thanks Mike Williams
     */
    typedef LoKi::Particles::PtFlight                                PTFLIGHT ;
    // ========================================================================
    /** @typedef BPVPTFLIGHT
     *  Simple functor to evaluate the transverse momentum with respect to 
     *  particle flight direction
     *  @see LoKi::Particles::PtFlightWithBestVertex 
     *  @see LoKi::Particles::PtFlight 
     *  @see LoKi::Particles::TransverseMomentumRel 
     *  @see LoKi::Cuts::PTFLIGHT
     *  @see LoKi::Cuts::PTDIR
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     *  @thanks Mike Williams
     */
    typedef LoKi::Particles::PtFlightWithBestVertex               BPVPTFLIGHT ;
    // ========================================================================
    /** @typedef CORRM 
     *  Simple functor to evaluate the 'corrected mass' with respect to 
     *  particle fligth direction
     * 
     *  @code 
     * 
     *   // get the primary vertex: 
     *   const LHCb::VertexBase* primary = ... ; 
     * 
     *   // create the functor 
     *   const CORRM fun =  CORRM ( primary ) ;
     * 
     *   const LHCb::Particle* B = ... ;
     *
     *   // use functor:
     *   const double mCorr = fun ( B ) ;
     *
     *  @endcode 
     *
     *  For more information see 
     *  <a href="http://indico.cern.ch/conferenceDisplay.py?confId=104542">
     *  Mike's slides at Trigger meeting 28 September 2k+10 </a>
     *
     *  @see LoKi::Particles::MCorrected
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     *  @thanks Mike Williams
     */
    typedef LoKi::Particles::MCorrected                                 CORRM ;
    // ========================================================================
    /** @typedef BPVCORRM 
     *  Simple functor to evaluate the corrected mass with respect to 
     *  particle flight direction
     *
     *  For more information see 
     *  <a href="http://indico.cern.ch/conferenceDisplay.py?confId=104542">
     *  Mike's slides at Trigger meeting 28 September 2k+10 </a>
     *
     *  @see LoKi::Particles::MCorrectedWithBestVertex
     *  @see LoKi::Particles::MCorrected
     *  @see LoKi::Particles::PtFlightWithBestVertex 
     *  @see LoKi::Particles::PtFlight 
     *  @see LoKi::Particles::TransverseMomentumRel 
     *  @see LoKi::Cuts::CORRM
     *  @see LoKi::Cuts::PTFLIGHT
     *  @see LoKi::Cuts::BPVPTFLIGHT
     *  @see LoKi::Cuts::PTDIR
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     *  @thanks Mike Williams
     */
    typedef LoKi::Particles::MCorrectedWithBestVertex                BPVCORRM ;
    // ========================================================================
    /** @typedef HOPM
     *  Simple functor to evaluate the HOP mass with respect to 
     *  particle flight direction
     *
     *  For more information see 
     *  <a href="https://cds.cern.ch/record/2102345/files/LHCb-INT-2015-037.pdf">
     *
     *  @see LoKi::Particles::MCorrectedWithBestVertex
     *  @see LoKi::Particles::MCorrected
     *  @see LoKi::Particles::PtFlightWithBestVertex 
     *  @see LoKi::Particles::PtFlight 
     *  @see LoKi::Cuts::CORRM
     *  @see LoKi::Cuts::PTFLIGHT
     *  @see LoKi::Cuts::BPVPTFLIGHT
     *  @authors Pavol Stefko pavol.stefko@epfl.ch, Guido Andreassi guido.andreassi@epfl.ch, Violaine Bellee violaine.bellee@epfl.ch
     *  @date   2017-01-17
     *  @thanks Albert Puig Navarro
     */
    typedef LoKi::Particles::BremMCorrected                              HOPM ;
    // ========================================================================
    /** @typedef BPVCORRM 
     *  Simple functor to evaluate the corrected mass with respect to 
     *  particle flight direction
     *
     *  For more information see 
     *  <a href="https://cds.cern.ch/record/2102345/files/LHCb-INT-2015-037.pdf">
     *
     *  @see LoKi::Particles::MCorrectedWithBestVertex
     *  @see LoKi::Particles::MCorrected
     *  @see LoKi::Particles::PtFlightWithBestVertex 
     *  @see LoKi::Particles::PtFlight 
     *  @see LoKi::Cuts::CORRM
     *  @see LoKi::Cuts::PTFLIGHT
     *  @see LoKi::Cuts::BPVPTFLIGHT
     *  @authors Pavol Stefko pavol.stefko@epfl.ch, Guido Andreassi guido.andreassi@epfl.ch, Violaine Bellee violaine.bellee@epfl.ch
     *  @date   2017-01-17
     *  @thanks Albert Puig Navarro
     */
    typedef LoKi::Particles::BremMCorrectedWithBestVertex             BPVHOPM ;
    // ========================================================================
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES38_H
// ============================================================================

