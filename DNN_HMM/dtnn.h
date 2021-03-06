// dtnn.h -- implementation of deep tensor neural network
//
// Dong Yu, Nov 2011
//
// $Log: /Speech_To_Speech_Translation/dbn/dbn/dtnn.h $
// 
// 56    5/05/15 10:40 Fseide
// backpropagationmodelupdate1() now also takes h as an input, as this is
// needed by the LSTM code;
// pushtorawgradient() also does the same;
// first version of LSTM is code-complete, although backprop and gradient
// computation have not at all been tested yet
// 
// 55    5/12/14 13:36 Fseide
// added infrastructure to no longer require dropout models to be scaled
// later (they are now scaled upon exitcomputation(), and unscaled upon
// entercomputation())--note: still need to fix the actual forwardprop()
// function;
// GenerateMeanModel() renamed to dropoutscaling() (dbn.h) and
// dropoutscaleweights (rbm.h) since it is now also used to undo the
// change
// 
// 54    3/20/14 4:07p Fseide
// some refactoring and initial code for local loop for data parallelism
// 
// 53    3/20/14 3:06p Fseide
// towards Kaldi-style model averaging for data parallelism
// 
// 52    2/21/14 13:50 Fseide
// backpropagationmodelupdate2() now takes 'bpinfo' like all others;
// implemented sub-gradient AdaGrad in there (to be tested)
// 
// 51    2/05/14 16:53 Fseide
// eliminated resetmomentum flag
// 
// 50    1/27/14 10:14 Fseide
// redefined currently unused function backpropagationmodelupdate2() to
// run after a deferred update but before aggregation, so that we can do
// AdaGrad scaling here.
// 
// 49    1/26/14 11:19 Fseide
// completely moved 'deferupdate' into dbn.h by just not calling aggregate
// and backpropagationmodelupdate3(), and rather have pushtorawgradient()
// and updategradient() control the state
// 
// 48    1/26/14 11:10 Fseide
// next step towards deferred update in raw gradient
// 
// 47    1/26/14 10:49 Fseide
// towards moving deferupdate to the raw gradient
// 
// 46    1/09/14 9:17 Fseide
// backpropagationmodelupdate2() now just takes the mpiaggregator directly
// (i.e it now knows that it is for this specific purpose)
// 
// 45    1/03/14 10:28 Fseide
// finally removed NEWGRADIENTSCALING as a #define-based option, we always
// use this anyway
// 
// 44    12/20/13 7:18p V-haofu
// fake split all other versions of backpropagationmodelupdate into 3
// functions;
// for each layer call backpropagationmodelupdate1..3 instead;
// 
// 43    9/29/13 13:18 Fseide
// now passes a buffer variable to matprod_mm() to support model
// parallelism
// 
// 42    8/28/13 10:55a Fseide
// added copyfrom() (as a dummy implementation)
// 
// 41    6/04/13 14:26 Fseide
// infrastructure for passing creation parameters to network
// instantiations (actual parsing and use to be implemented)
// 
// 40    6/02/13 13:53 Fseide
// replaced two overloads of write() with a function template
// 
// 39    1/03/13 8:53p Kaisheny
// Asynchronous SGD using data pipe.
// 
// 38    11/16/12 5:48p Fseide
// forwardpropdelta() now returns its 'eps', which is then passed to
// compensationupdate()
// 
// 37    11/16/12 5:34p Fseide
// forwardpropdelta() now takes learning rate per frame and momentum per
// sample;
// so does unseenstatecompensaion(), but that is not fully implemented yet
// 
// 36    11/16/12 4:17p V-hansu
// modify the forwardpropdelta(), seems done, not tested
// 
// 35    11/16/12 11:02a V-hansu
// add eh into forwardpropdelta() since it will be used
// 
// 34    11/16/12 10:50a V-hansu
// modify scalederrorE to layerstatenorm, and scaleE to vnorms, add method
// allocatestatevectors()
// 
// 33    11/15/12 7:35p V-hansu
// add another matrixref scaledE in forwardpropdelta() to save middle
// results
// 
// 32    11/14/12 7:26p V-hansu
// add forwardpropdelta(), not finished
// 
// 31    10/12/12 1:48p Dongyu
// added support of dropout training for DNN (frame level training only). 
// addes support to convert the model based on dropout rate used in the
// training and/or senone sections used in multilingual training.
// 
// 30    10/10/12 10:01a Dongyu
// added support to train models that shares the same hidden layers but
// use different senone sets from different langauges. This allows us to
// train universal ASR with separate senonoes or use models trained using
// multiple languages to adapt to new langauges.
// 
// 29    10/09/12 6:44p Fseide
// changed rand() to ::rand() because there was some conflict
// 
// 28    9/23/12 8:01p Fseide
// const correctness for modelupdateinfo parameters
// (backpropagationmodelupdate(), updatedeltas(), preflayer)
// 
// 27    9/02/12 6:19p Fseide
// implemented I-smoothing
// 
// 26    9/02/12 5:47p Fseide
// regularizationtype::alpha renamed to more concise L2weightpersample
// 
// 25    9/02/12 5:06p Fseide
// addweighted() now takes a 'thisscale' parameter, in prep for L2
// regularization
// 
// 24    8/07/12 18:14 Fseide
// completed implementation of deferupdate flag (still to be tested)
// 
// 23    8/07/12 17:46 Fseide
// new option to backpropagationmodelupdate(): deferupdate, used to
// implement batches of batches, for an MMI experiment
// 
// 22    7/19/12 11:44a V-hansu
// deleted the getroundup function
// 
// 21    7/06/12 9:16p V-hansu
// modify the inferface of blowup again to ge compatible
// 
// 20    7/05/12 8:00p V-hansu
// chang the interface of blow up to let it able to return roundup unit
// 
// 19    7/02/12 4:27p V-hansu
// add function setlinearlayerweight to be compatible with rbm
// 
// 18    6/27/12 9:23p V-hansu
// add a print(FILE *f) function which is not completed
// 
// 17    6/22/12 2:13p V-hansu
// changed the interface of function blowup
// 
// 16    6/22/12 9:25a V-hansu
// add another blowup function to be compatible with base class
// 
// 15    6/05/12 2:01p V-hansu
// add a virtual blowup function
// 
// 14    5/31/12 10:54p V-xieche
// fix a bug in exitcomputation function for more than 2 cuda devices on
// top layer
// 
// 13    5/13/12 10:59p V-xieche
// add initial code to make toplayer support more than 2 cuda devices in
// pipeline training. not finish yet
// 
// 12    4/03/12 8:31p V-xieche
// check in all code for supporting pipeline training. striped top layer
// in two cuda devices. add some virtual functions to make it compilable,
// not modify it.
// 
// 11    3/27/12 11:30 Fseide
// fixed signed/unsigned warnings
// 
// 10    3/27/12 1:09a V-xieche
// Add codes for multi devices. 
// 
// 9     3/11/12 7:05p V-xieche
// add code for a compact trainer. make it run in CUDA directly.
// 
// 8     3/08/12 10:34p V-xieche
// add some virtual functions in dtnn class to make it compilable.
// 
// 7     3/01/12 7:27p V-xieche
// add virtual function in Iannlayer class to make the code compilable for
// flatten sigmoid training.
// 
// 6     2/07/12 2:28p Dongyu
// fixed momentum invalid problem when learning rate is 0
// 
// 5     12/20/11 3:58p Dongyu
// verified that dtnn training works.
// 
// 4     12/07/11 4:25p Dongyu
// fully implemented dtnn
// 
// 3     11/29/11 11:11a F-gli
// add peekweightmatrix() peekbias() forwardpropwithoutnonlinearity() to
// Iannlayer class
// 
// 2     11/28/11 5:54p Dongyu
// implemented backprop for dtnn
// 
// 1     11/23/11 4:27p Dongyu
// added dtnn class. many functions are not implemented yet.
// 

#pragma once

#include "rbm.h"
#include "ssematrix.h"          // for basic matrix type
#include "parallelrbmmatrix.h"  // for parallel accelerated matrix operations (NUMA, CUDA)
#include <string>
#include <stdexcept>

#define DETAILDEBUGINFO

namespace msra { namespace dbn {

class dtnn : public Iannlayer
{
private:
    dtnn (const dtnn &) {};
    void operator= (const dtnn &) {};

protected:
    rbmmodelmatrix W1;   // weight for tensor output 1
    rbmmodelmatrix a1;   // bias for tensor output 1
    rbmmodelmatrix W2;   // weight for tensor output 2
    rbmmodelmatrix a2;   // bias for tensor output 2

    rbmmodelmatrix dW1;  // derivative at last call to -update(); used for momentum
    rbmmodelmatrix da1;  // Note that these deltas are scaled by 1/(1-momentum), for code simplicity.
    rbmmodelmatrix dW2;  // derivative at last call to -update(); used for momentum
    rbmmodelmatrix da2;  // Note that these deltas are scaled by 1/(1-momentum), for code simplicity.

    mutable rbmstatevectors h1matrix;
    mutable rbmstatevectors h2matrix;

    mutable rbmstatevectors eh1matrix;
    mutable rbmstatevectors eh2matrix;

    mutable cachedmatrix cachedW1s;
    mutable cachedmatrix cacheda1s;
    mutable cachedmatrix cachedW2s;
    mutable cachedmatrix cacheda2s;

    mutable cachedmatrix cachedvs;
    mutable cachedmatrix cachedh1s;
    mutable cachedmatrix cachedh2s;
    mutable cachedmatrix cachedhs;

    mutable cachedmatrix cachedW1ts;
    mutable cachedmatrix cachedW2ts;

    // storage for updatedeltas() 
    vector sumh1tmp;
    vector sumh2tmp;
    vector sumvtmp;
    mutable matrix h1ttmp;
    mutable matrix h2ttmp;
    mutable cachedmatrix cachedh1ts;
    mutable cachedmatrix cachedh2ts;
    mutable cachedmatrix cachedvts;
    mutable cachedmatrix cachedehts;

public:
#if 0
    dtnn (matrix && pW1, vector && pa1, matrix && pW2, vector && pa2)
    {
        W1 = std::move (pW1);
        a1 = std::move (pa1);
        W2 = std::move (pW2);
        a2 = std::move (pa2);
    }
#endif

    dtnn (size_t vdim, size_t h1dim, size_t h2dim, const layerconfigparameters & c, unsigned int randomseed)
    {
        W1.resize (vdim, h1dim);
        a1.resize (h1dim, 1);
        W2.resize (vdim, h2dim);
        a2.resize (h2dim, 1);

        validatedims();

        initrandom (randomseed);
    }

    dtnn (FILE * f) 
    { 
        W1.read (f, "W1");
        a1.read (f, "a1");
        W2.read (f, "W2");
        a2.read (f, "a2");

        validatedims(); 
    }

    dtnn (const HANDLE f) 
    { 
        W1.read (f, "W1");
        a1.read (f, "a1");
        W2.read (f, "W2");
        a2.read (f, "a2");

        validatedims(); 
    }

    virtual string type() const { return "dtnn"; }

    virtual void copyfrom (const Iannlayer & other) { throw std::logic_error ("copyfrom: not yet implemented for this type of layer"); }

    // special-purpose accessors (it's a research project after all...)
    const matrix & peekW1() const { return W1.peek(); }
    const vector & peeka1() const { return a1.peek(); }

    const matrix & peekW2() const { return W2.peek(); }
    const vector & peeka2() const { return a2.peek(); }

    virtual void print() const
    {
        printmat(W1);
        printmat(a1);

        printmat(W2);
        printmat(a2);
    }

    virtual void print(FILE *f) const
    {
        throw std::logic_error ("print(FILE *f): not implemented");
    }

    virtual const matrix & peekweightmatrix() const
    {
        throw std::logic_error ("peekweightmatrix: not implemented");
    }

    virtual const vector & peekbias() const 
    {
        throw std::logic_error ("peekbias: not implemented");
    }

    virtual void forwardpropwithoutnonlinearity (const matrixstripe & v, matrixstripe & u, size_t i) const 
    {
        throw std::logic_error ("forwardpropwithoutnonlinearity: not implemented");
    }

    
    // print model stats
    // Returns a pair (total model params, total non-null model params).
    virtual pair<unsigned int,unsigned int> printvaluedistribution (const string & tag) const
    {
        auto W1stats = msra::math::printmatvaluedistributionf (("W1 " + tag).c_str(), W1);
        auto a1stats = msra::math::printmatvaluedistributionf (("a1 " + tag).c_str(), a1);

        auto W2stats = msra::math::printmatvaluedistributionf (("W2 " + tag).c_str(), W2);
        auto a2stats = msra::math::printmatvaluedistributionf (("a2 " + tag).c_str(), a2);

        return make_pair (W1stats.first + a1stats.first + W2stats.first + a2stats.first, W1stats.second + a1stats.second + W2stats.second + a2stats.second);
    }

    // I/O
    // This is virtual to allow networks to save network-type specific data.
    // 'Overridden' reading is done in the constructor from FILE *.
    template<typename FILEHANDLETYPE>
    void dowrite (FILEHANDLETYPE f) const
    {
        W1.write (f, "W1");
        a1.write (f, "a1");

        W2.write (f, "W2");
        a2.write (f, "a2");
    }
    virtual void write (FILE * f) const { dowrite (f); }
    virtual void write (HANDLE f) const { dowrite (f); }

    virtual void dropoutscaleweights (float factor)
    {
        W1.scale (factor);
        W2.scale (factor);
    }

    // get the dimensions
    virtual size_t vdim() const { return W1.rows(); }
    virtual size_t hdim() const { return W1.cols() * W2.cols(); }
    virtual std::vector<size_t> hdims() const
    {
        std::vector<size_t> hdimsvec(2);
        hdimsvec[0] = W1.cols();
        hdimsvec[1] = W2.cols();

        return hdimsvec;
    }

    // do necessary preparations to start any computation with the model
    // 'type'can be:
    //  -1 -> backpropagation
    //  +1 -> pretraining
    //   0 -> evaluation
    // With CUDA, this loads the model into the CUDA RAM.
    virtual void entercomputation (int type)
    {
        W1.entercomputation(); a1.entercomputation(); 
        W2.entercomputation(); a2.entercomputation(); 

        // this is a good time to lazily allocate the delta matrices
        if (type != 0)
        {
            dW1.resize (W1.rows(), W1.cols());
            da1.resize (a1.rows(), a1.cols()); // (a.cols()==1, it's a vector)

            dW2.resize (W2.rows(), W2.cols());
            da2.resize (a2.rows(), a2.cols()); // (a.cols()==1, it's a vector)
            // note: the first time, entercomputation() calls below will copy garbage
        }
        dW1.entercomputation(); da1.entercomputation(); 
        dW2.entercomputation(); da2.entercomputation(); 
        // prepare accumulators for training
        if (type != 0)
        {
            fprintf (stderr, "entercomputation: zeroing gradient accumulators\n");  // (can remove this message one day)
            dW1.setzero(); da1.setzero();
            dW2.setzero(); da2.setzero();
        }
    }
#ifdef MULTICUDA
    virtual void entercomputation (int type, size_t deviceid)
	{
		throw runtime_error ("Should not call compact trainer version entercomputattion in dtnn class!\n");
	}
    virtual void entercomputation (int type, size_t deviceid, bool stripedmode, size_t devnum)
    {
        throw runtime_error ("Should not call compact trainer version entercomputattion WITH striped mode in dtnn class!\n");
    }
    virtual void exitcomputation (size_t deviceid, bool stripedmode, size_t topdevicenum)
    {
        throw runtime_error ("Should not call compact trainer version exitcomputation WITH striped in dtnn class!\n");
    }
	virtual void exitcomputation (size_t deviceid)
	{
		throw runtime_error ("Should not call compact trainer version exitcomputation in dtnn class!\n");
	}
	virtual float getweightvalue(size_t c, size_t l) const
	{
		throw runtime_error ("Shoule not call getweightvalue function for compacttrainer.\n");
		return 0.0;
	}
#endif

    // same do necessary finalization, e.g. in case of CUDA, copy updated models back to CPU RAM
    virtual void exitcomputation()
    {
        W1.exitcomputation(); a1.exitcomputation(); 
        W2.exitcomputation(); a2.exitcomputation(); 

        dW1.exitcomputation(); da1.exitcomputation(); 
        dW2.exitcomputation(); da2.exitcomputation(); 
    }

    virtual void validatedims() const   // check if dimensions match
    {
        if (W1.cols() != a1.rows())
            malformed ("invalid model file -- W1.cols() != a1.rows()");

        if (W2.cols() != a2.rows())
            malformed ("invalid model file -- W2.cols() != a2.rows()");

        if (W1.rows() != W2.rows())
            malformed ("invalid model file -- W1.rows() != W2.rows()");
    }

    // helper for constructors --reset W, and a with random values
    virtual void initrandom (unsigned int randomseed)
    {
        srand (randomseed);

        foreach_coord (i, j, W1)
            W1(i,j) = (::rand() * 0.1f / RAND_MAX) - 0.05f;
        foreach_row (i, a1)
            a1[i] = 0.0f;

        foreach_coord (i, j, W2)
            W2(i,j) = (::rand() * 0.1f / RAND_MAX) - 0.05f;
        foreach_row (i, a2)
            a2[i] = 0.0f;
    }

    virtual void blowup(const size_t blowupfactor)    // added by Hang Su adaptation
    {
        if (blowupfactor != 1 )
            throw std::logic_error("blowup not implemented for DTNN!\n");
    }

    virtual void setlinearlayerweight(const matrix & adaptmatrix)
    {
        throw std::logic_error ("setlinearlayerweight: not implemented for dtnn");
    }

    virtual void blowup(const size_t blowupfactor, const std::vector<size_t> & statemap)        // added by Hang Su adaptation
    {
        throw runtime_error("blowup with statemap not implemented for DTNN!\n");
    }

     // split (double-up) nodes
    // 'out' true means double the output nodes, else double the input nodes.
    // This is only defined for the forward direction. b is just updated in terms of dimension.
    // So far identical for all nodes. Change to virtual if not.
    virtual void doublenodes (bool out)
    {
        srand ((unsigned int) W1.rows());
        // double output nodes: perturb a little
        if (out)
        {
            doubleoutnodes(W1, a1);
            doubleoutnodes(W2, a2);
        }
        // double input nodes: half the weights
        // Weights are halved because each input now exists twice (except small perturbance)
        else
        {
            doubleinnodes(W1);
            doubleinnodes(W2);
        }
    }

    void dumplayer() const
    {
        fprintf(stderr, "[ ");
        foreach_coord(i, j, W1)
        {
            if(i == 0 && j > 0)   fprintf(stderr, ";\n");
            fprintf(stderr, "%.4f ", W1(i, j));
        }
        fprintf(stderr, ";]\n");

        fprintf(stderr, "[ ");
        foreach_coord(i, j, W2)
        {
            if(i == 0 && j > 0)   fprintf(stderr, ";\n");
            fprintf(stderr, "%.4f ", W2(i, j));
        }
        fprintf(stderr, ";]\n");
    }

    virtual size_t getnumberofweightsets() const {return 2;}

    virtual pair<size_t, size_t> getweightsetdims(const size_t weightsetindex) const 
    {
        assert (weightsetindex < getnumberofweightsets());

        if (weightsetindex == 0) 
        {
            pair<size_t, size_t> p(W1.rows(), W1.cols());
            return p;
        }
        else 
        {
            pair<size_t, size_t> p(W2.rows(), W2.cols());
            return p;
        }
    }

    // set weights (this is to support hack experiments)
    template<class WTYPE, class ATYPE>
    void setweights (const WTYPE & newW, const ATYPE & newa, const size_t weightsetindex)
    {
        assert (weightsetindex == 0 || weightsetindex == 1);
        if (weightsetindex == 0)
        {
            assert (newW.rows() == W1.rows() && newW.cols() == W1.cols());
            assert ((size_t) newa.size() == a1.rows());
            foreach_coord (i, j, W1)
                W1(i,j) = (float) newW(i,j);
            foreach_index (i, newa)
                a1[i] = (float) newa[i];
        }
        else
        {
            assert (newW.rows() == W2.rows() && newW.cols() == W2.cols());
            assert ((size_t) newa.size() == a2.rows());
            foreach_coord (i, j, W2)
                W2(i,j) = (float) newW(i,j);
            foreach_index (i, newa)
                a2[i] = (float) newa[i];
        }
    }

    // forward propagation
    // v and Ph are blocks of column vectors
    virtual void forwardprop (const rbmstatevectorsref & v, rbmstatevectorsref & h, const bool linearonly = false, rbmstatevectorsref * pmask=NULL) const
    {
        if (linearonly) throw std::logic_error ("forwardprop: linear only is not implemented yet for dtnn layer");
        if (pmask!=NULL) throw std::logic_error ("forwardprop: non-null mask is not implemented yet for dtnn layer");

        allocforwardtemps(h.cols());
        
        rbmstatevectorsref h1(h1matrix.stripe(0, h1matrix.cols()));
        rbmstatevectorsref h2(h2matrix.stripe(0, h2matrix.cols()));

        vtoh (v, h1, h2, h);
    }

    // backward error propagation
    // must be called after forward pass which will call allocforwardtemps
    // h1 and h2 are already after sigmoid
    virtual void backpropagationstats (rbmstatevectorsref & /*in/out*/ eh, const rbmstatevectorsref & h,  rbmstatevectorsref & /*out*/ ev) const
    {
        assert (eh.cols() == h.cols() && eh.rows() == h.rows());

        allocbackwardtemps(h.cols());

        //eh.reshape(h1dim(), h2dim());     //for each column 
        //eh1 = eh * h2;   
        //eh2 = eh' * h1;
        
        rbmstatevectorsref h1(h1matrix.stripe(0, h1matrix.cols()));
        rbmstatevectorsref h2(h2matrix.stripe(0, h2matrix.cols()));
        rbmstatevectorsref eh1(eh1matrix.stripe(0, eh1matrix.cols()));
        rbmstatevectorsref eh2(eh2matrix.stripe(0, eh2matrix.cols()));

        eh1.reshapecolumnproduct(eh, h2, false);
        eh2.reshapecolumnproduct(eh, h1, true);

        // dW <- dW * feedbackweight + sum (v * h') * inputweight

        // compute 'dW' = [ dh/d(w(i,j)) ] and 'da' = [ dh/d(a[i]) ]
        // eh = hdesired - h
        // err = eh .* h .* (1 - h)
        // h .* (1 - h) = derivative of sigmoid
        // update 'eh' in place for later use in accumulation

        // multiply by derivative
        // This is done in place because we need the very same product later in the model update.
        // Note that eh no longer corresponds to e^l in the paper after this operation.
        eh1.mulbydsigm (h1);
        eh2.mulbydsigm (h2);

        //// return value 'ev'  are error back-propagated through network, to pass to next lower layer
        if (!ev.empty() )    // (bottom level does not need this--pass an empty matrix)
            ehtoev (eh1, eh2, ev);  // ev = W eh  (eh is the updated one)
    }

    // v-hansu not finished
    virtual float forwardpropdelta (rbmstatevectorsref & deltah, const rbmstatevectorsref & deltav, const rbmstatevectorsref & h, 
                                    /*const*/ rbmstatevectorsref & v, /*const*/ rbmstatevectorsref & eh, rbmstatevectorsref & vnorms,
                                    const float learningrateperframe, const double momentumpersample) const
    {
        throw std::logic_error ("forwardpropdelta: not implemented yet for dtnn layer");
    }

    // for dtnns, we do all in backpropagationmodelupdate3()
    // This means, o.a., that we don't support MPI.
	virtual void backpropagationmodelupdate1(const rbmstatevectorsref & ehxs, const rbmstatevectorsref & v, const modelupdateinfo & bpinfo){}
    virtual void backpropagationmodelupdate1 (const rbmstatevectorsref & ehxs, const rbmstatevectorsref & v, const rbmstatevectorsref & h, const modelupdateinfo & bpinfo) { }

    virtual void backpropagationmodelupdate2 (const modelupdateinfo & bpinfo, bool mpimaisfirst, bool mpimaislast, float learningratepersample, double momentumpersample) { }

    virtual void backpropagationmodelupdate3 (const rbmstatevectorsref & ehxs,  const rbmstatevectorsref & v,
                                             float learningratepersample, double momentumpersample, const modelupdateinfo & bpinfo)
    {
        backpropagationmodelupdate (ehxs, v, learningratepersample, momentumpersample, bpinfo);
    }
    
    // perform model update (back-propagation)
    // 'ehxs' is the error signal multiplied with the sigmoid' (except for linear fDLR layer)
    // This is the default implementation. 'linearnetwork' has its own, hence the virtual function.
    virtual void backpropagationmodelupdate (const rbmstatevectorsref & ehxs, const rbmstatevectorsref & v,
                                             float learningratepersample, double momentumpersample, const modelupdateinfo & bpinfo)
    {
        const size_t mbsize = v.cols(); 
        assert (ehxs.cols() == mbsize);

        const float momentum = (float) exp (log (momentumpersample) * mbsize);  // map momentum to actual mb size  --compatible mode; will change

        const float gradientscaling = learningratepersample;    // learning rate is applied to gradients before momentum smoothing for consistency
        const float inputweight = (1.0f - momentum) * gradientscaling;
        const float gradientweight = gradientscaling<1e-30? 1:learningratepersample / gradientscaling;

        updatedeltas (momentum, v, inputweight, bpinfo);
        adddeltas (gradientweight);

        if (bpinfo.sparsethreshold > 0)                // make weights sparse
            sparsifyweights (bpinfo.sparsethreshold);
    }
    
    virtual void pretrainingstats (const rbmstatevectorsref & Ph, rbmstatevectorsref & v1, rbmstatevectorsref & h1, unsigned int randomseed) const
    {
    }

    virtual void pretrainingmodelupdate (const rbmstatevectorsref & v, const rbmstatevectorsref & h, rbmstatevectorsref & v1, rbmstatevectorsref & h1,
                                         float learningrate, double momentumpersample)
    {
    }
#ifdef COMPACTTRAINER  // for compact trianer. [v-xieche]
    virtual msra::cuda::matrix & getcudaweight (size_t deviceid)
    {
        throw runtime_error("Should not get here by using getcudaweight function in dtnn!\n");
        return W1.getcudamatrix (deviceid);
    }
#ifdef STRIPEDTOPLAYER
    virtual msra::cuda::matrix  & stripedgetcudaweight (size_t devid, size_t devnum, msra::dbn::cudadistributedmatrix::cudastriping_t s)
    {
        throw runtime_error ("Should not get here by uisng get weight WITH stripe mode in dtnn!\n");
        return W1.getcudamatrix (devid);
    }
    virtual msra::cuda::matrix & stripedgetcudabias(size_t devid, size_t devnum, msra::dbn::cudadistributedmatrix::cudastriping_t s)
    {
        throw runtime_error("Should not get here by using getcudabias function in dtnn!\n");
        return a1.getcudamatrix (devid);
    }
#endif
    virtual msra::cuda::matrix & getcudabias (size_t deviceid)
    {
        throw runtime_error("Should not get here by using getcudabias function in dtnn!\n");
        return a1.getcudamatrix (deviceid);
    }

    virtual rmbmodelmatrix & getweight ()
    {
        throw runtime_error("Should not get here by using getweight function in dtnn!\n");
        return W1;
    }

    virtual rmbmodelmatrix & getbias ()
    {
        throw runtime_error("Should not get here by using getbias function in dtnn!\n");
        return a1;
    }

    virtual void backpropagationmodelupdateincuda (const rbmstatevectorsref & ehxs,  const rbmstatevectorsref & v,
                                             float learningratepersample, double momentumpersample, bool resetmomentum, modelupdateinfo & bpinfo, size_t deviceid)
    {
        throw runtime_error("Should not get here by using backpropagationmodelupdateincuda function in dtnn!\n");
    }
    virtual void backpropagationmodelupdatestripedincuda (const rbmstatevectorsref & ehxs,  const rbmstatevectorsref & v,
                                             float learningratepersample, double momentumpersample, bool resetmomentum, modelupdateinfo & bpinfo, size_t deviceid, size_t devnumusedintoplayer = 2)
    {
        throw runtime_error("Should not get here by using backpropagationmodelupdatestripedincuda function in dtnn!\n");
    }
#endif

#ifdef STEEPERSIGMOID
    virtual void multiplywith (float n)
    {
        throw runtime_error("Should not get here by using multiplywith function in dtnn!\n");
    }
    virtual float getweightvalue(size_t c, size_t l) const
    {
        throw runtime_error("Should not get here by using getweightvalue function in dtnn!\n");
    }
    virtual float getbiasvalue(size_t m) const
    {
        throw runtime_error("Should not get here by using getbiasvalue function in dtnn!\n");
    }
#endif


protected:

    size_t h1dim() const { return W1.cols(); }
    size_t h2dim() const { return W2.cols(); }

    void allocforwardtemps (size_t framesinblock) const
    {
        if (h1matrix.rows() != W1.cols() || h1matrix.cols() != framesinblock)
        {
            h1matrix.resize (W1.cols(), framesinblock);
        }

        if (h2matrix.rows() != W2.cols() || h2matrix.cols() != framesinblock)
        {
            h2matrix.resize (W2.cols(), framesinblock);
        }    
    }

    void allocbackwardtemps (size_t framesinblock) const
    {
        if (eh1matrix.rows() != W1.cols() || eh1matrix.cols() != framesinblock)
        {
            eh1matrix.resize (W1.cols(), framesinblock);
        }

        if (eh2matrix.rows() != W2.cols() || eh2matrix.cols() != framesinblock)
        {
            eh2matrix.resize (W2.cols(), framesinblock);
        }    
    }

    void doubleoutnodes(rbmmodelmatrix & W, rbmmodelmatrix & a)
    {
        matrix newW (W.rows(), 2 * W.cols());
        vector newa (2 * W.cols());
        foreach_column (j, W)
        {
            // Note: 'eps' might have to depend on the actual scale of the column,
            // which impacts the slope of the sigmoid. Now I just try to make it 'small'.
            foreach_row (i, W)
            {
                const float eps = ::rand() * 0.01f / RAND_MAX;
                newW(i,2*j)   = W(i,j) + eps;
                newW(i,2*j+1) = W(i,j) - eps;
            }
            newa[2*j]   = a[j];
            newa[2*j+1] = a[j];
        }
        W = std::move (newW);
        a = std::move (newa);
    }

    //BUGBUG: this implementation does take into account the fact previous H is a cross product of h1 and h2.
    //don't use it
    void doubleinnodes(rbmmodelmatrix & W)  
    {
        matrix newW (2 * W.rows(), W.cols());
        foreach_row (i, W)
        {
            foreach_column (j, W)
            {
                newW(2*i,j)   = W(i,j) * 0.5f;
                newW(2*i+1,j) = W(i,j) * 0.5f;
            }
        }
        W = std::move (newW);
    }

    // update the deltas, with momentum
    //  - momentum is a 1st-order IIR low-pass, with non-unity gain G  --TODO: clean up these comments w.r.t. weight
    //    y(t+1) = momentum * y(t) + (1-momentum) * x(t) * G
    //    G=1/(1-momentum)
    //     - previous deltas are weighted down by feedbackweight, e.g. 0.9
    //     - gradient is added to it
    //       NOTE: gradient is weighted by (1-momentum) * G = 1, i.e. not weighted --we save a multiplication.
    //       Thus, the smoothed gradients are all too large by G=1/(1-momentum).
    //       So, remember to correct that when adding the smoothed gradients to the model parameters.
    //  - feedbackweight == 0 is used to reset the momentum (equivalent to x(t) = 0 for t < 0)
    // used for:
    //  - pretraining, second summand (negative) comes first: pass -momentum to flip sign
    //  - pretraining, first summand (positive): pass feedbackweight=-1.0 to flip sign once more
    //  - finetuning: also applies momentum, h is actually the error signal for h multiplied with the sigmoid' (except for linear fDLR layer)
    // Call this only on a single thread. Relies on keeping storage variables above around for avoiding mem alloc operations.
    void updatedeltas (const float feedbackweight, const rbmstatevectorsref & v, const float inputweight, const modelupdateinfo & bpinfo)
    {
        // da <- da * feedbackweight + sum (h)      * inputweight
        // dW <- dW * feedbackweight + sum (v * h') * inputweight
        // where h and v are matrices with columns = frames.
        // the momentum filter is implemented as:
        //  - feedbackweight = momentum
        //  - inputweight    = (1-momentum)

        // bias vectors
        assert (!da1.empty() && !dW1.empty() && !da2.empty() && !dW2.empty() );

        rbmstatevectorsref eh1(eh1matrix.stripe(0, eh1matrix.cols()));
        rbmstatevectorsref eh2(eh2matrix.stripe(0, eh2matrix.cols()));
        assert (eh1.cols() == v.cols() && eh2.cols() == v.cols());  // cols = frames

        dW1.scaleandaddmatprod (feedbackweight, v, eh1, inputweight, h1ttmp, cachedvts, cachedh1ts);
        da1.scaleandaddallcols (feedbackweight, eh1, inputweight, sumh1tmp);
        dW2.scaleandaddmatprod (feedbackweight, v, eh2, inputweight, h2ttmp, cachedvts, cachedh2ts);
        da2.scaleandaddallcols (feedbackweight, eh2, inputweight, sumh2tmp);

        // compute the deltas; keep previous deltas as "momentum"
        // Note: smoothed gradients are scaled by 1/(1-momentum).
        if (bpinfo.regtype == regCL || bpinfo.regtype == regCS)
        {

            const float threshold = fabs (bpinfo.nochangeifaboveorbelow);
            if (threshold > 0)
            {
                // TODO: interplay with momentum not clear
                if (bpinfo.nochangeifaboveorbelow > 0) 
                {
                    dW1.setto0ifabsabove2 (W1, threshold);
                    dW2.setto0ifabsabove2 (W2, threshold);
                }
                else
                {
                    dW1.setto0ifabsbelow2 (W1, threshold);
                    dW2.setto0ifabsbelow2 (W2, threshold);
                }
            }
        }
        else if (bpinfo.regtype == regL2 || bpinfo.regtype == regIsmoothing)
        {
            const dtnn & dtnnlayer = dynamic_cast<const dtnn &> (*bpinfo.preflayer);
            const rbmmodelmatrix & W1ref = dtnnlayer.W1;
            const rbmmodelmatrix & a1ref = dtnnlayer.a1;
            const rbmmodelmatrix & W2ref = dtnnlayer.W2;
            const rbmmodelmatrix & a2ref = dtnnlayer.a2;

            const float alpha = bpinfo.L2weightpersample * v.cols() * inputweight;  //adjust based on number of frames

            assert(W1.rows() == W1ref.rows());
            assert(W1.cols() == W1ref.cols());
            assert(a1.rows() == a1ref.rows());
            assert(a1.cols() == a1ref.cols());

            assert(W2.rows() == W2ref.rows());
            assert(W2.cols() == W2ref.cols());
            assert(a2.rows() == a2ref.rows());
            assert(a2.cols() == a2ref.cols());

            // dW += alpha * (Wref - Wcur)
            // TODO: alpha interplays with scaling of gradient --ensure it is correct
            if (bpinfo.regtype == regL2)
            {
                dW1.addweighted (1.0f, W1, -alpha);
                da1.addweighted (1.0f, a1, -alpha);
                dW2.addweighted (1.0f, W2, -alpha);
                da2.addweighted (1.0f, a2, -alpha);
            }

            dW1.addweighted (1.0f, W1ref, alpha);
            da1.addweighted (1.0f, a1ref, alpha);
            dW2.addweighted (1.0f, W2ref, alpha);
            da2.addweighted (1.0f, a2ref, alpha);
        }
    }


    // add deltas to parameters
    //  - deltas are already summed over the frames of the minibatch
    //  - 'gradientweight' parameter combines
    //     - per-frame learning rate
    //     - momentum complement (1-momentum) since smoothed gradients are too large by 1/(1-momentum),
    //       see comment at updatedeltas
    void adddeltas (float gradientweight)
    {
        // W1 += dW1 * learning rate
        // a1 += da1 * learning rate
        
        //peekrbmmodelmatrix(dW1, "dW1\n",  10, 2);
        //peekrbmmodelmatrix(W1, "W1 before W1.addweighted\n",  10, 2);

        W1.addweighted (1.0f, dW1, gradientweight);
        a1.addweighted (1.0f, da1, gradientweight);

        //peekrbmmodelmatrix(W1, "W1 after W1.addweighted\n",  10, 2);

        W2.addweighted (1.0f, dW2, gradientweight);
        a2.addweighted (1.0f, da2, gradientweight);
    }

    // force sparseness to parameters
    void sparsifyweights (float threshold) 
    {
        W1.setto0ifabsbelow (threshold);
        W2.setto0ifabsbelow (threshold);
    }

    // apply the weight matrix to v plus bias to get h
    void vtoh (const rbmstatevectorsref & v, rbmstatevectorsref & h1, rbmstatevectorsref & h2, rbmstatevectorsref & h) const
    {
        W1.matprod_mtm (v, cachedW1s, cachedvs, h1, cachedh1s, a1, cacheda1s);     // h1 = sigmoid(W1' v + a1)
        h1.sigmoid();

        W2.matprod_mtm (v, cachedW2s, cachedvs, h2, cachedh2s, a2, cacheda2s);     // h2 =sigmoid( W2' v + a2)
        h2.sigmoid();

        h.KhatriRaoProduct(h1, h2);    // h = h1 h2' (khatri-Raw product)
    }

    mutable rbmstatevectors ehtoevexchangebuffer;

    // apply weights to error signal in reverse direction for error back-propagation
    // Difference to htov() is that no bias is added as this deals with error signals.
    void ehtoev (const rbmstatevectorsref & eh1, const rbmstatevectorsref & eh2, rbmstatevectorsref & ev) const
    {
        assert (eh1.cols() == ev.cols() && eh2.cols() == ev.cols());
        W1.matprod_mm (eh1, cachedW1ts, cachedh1s, ev, cachedvs, ehtoevexchangebuffer, 0.0f);   // ev = W1 eh1, note eh1 is actuall diag(sigmoid'(z)) h when passed in
        W2.matprod_mm (eh2, cachedW2ts, cachedh2s, ev, cachedvs, ehtoevexchangebuffer, 1.0f);   // ev += W2 eh2
    }

    //void peekrbmstatevectors(const rbmstatevectors& m, std::string msg, const size_t colstoshow) const
    //{   
    //    fprintf(stderr, msg.c_str());
    //    const rbmstatevectorsrefreading r(m, 0, m.cols());
    //    for (int i=0;i<r.rows(); i++)
    //    {
    //        for (int j=0;j<min(colstoshow, r.cols()); j++)
    //        {
    //            fprintf(stderr, "%f  ", r(i,j));
    //        }
    //        fprintf(stderr, "\n");
    //    }
    //}

    void peekrbmstatevectorrefs(const rbmstatevectorsref& m, std::string msg, const size_t rowstoshow, const size_t colstoshow) const
    {   
        fprintf(stderr, msg.c_str());
        m.lockforreading();
        for (size_t i = 0; i < min(rowstoshow, m.rows()); i++)
        {
            for (size_t j = 0; j < min(colstoshow, m.cols()); j++)
            {
                fprintf(stderr, "%f  ", m(i,j));
            }
            fprintf(stderr, "\n");
        }
        m.unlock();
    }

    void peekrbmmodelmatrix(rbmmodelmatrix& mat, std::string msg, const size_t rowstoshow, const size_t colstoshow) const
    {   
        fprintf(stderr, msg.c_str());
        mat.exitcomputation();
        const matrix m = mat.peek();
        for (size_t i = 0; i < min(rowstoshow, m.rows()); i++)
        {
            for (size_t j = 0; j < min(colstoshow, m.cols()); j++)
            {
                fprintf(stderr, "%f  ", m(i,j));
            }
            fprintf(stderr, "\n");
        }
        mat.entercomputation();
    }

    static void malformed (std::string msg) { throw std::runtime_error ("dtnn: invalid model file: " + msg); }

};
};};

//test cases 
/*
A =
    0.8147    0.9134    0.2785    0.9649
    0.9058    0.6324    0.5469    0.1576
    0.1270    0.0975    0.9575    0.9706

B =
    0.9572    0.8003    0.4218    0.7922
    0.4854    0.1419    0.9157    0.9595

C = KhatriRaoProduct(A,B);

    0.7798    0.7310    0.1175    0.7644
    0.8670    0.5061    0.2307    0.1249
    0.1215    0.0781    0.4038    0.7689
    0.3954    0.1296    0.2550    0.9258
    0.4396    0.0897    0.5008    0.1512
    0.0616    0.0138    0.8768    0.9313

=========================================
eh =
    0.6557    0.7431
    0.0357    0.3922
    0.8491    0.6555
    0.9340    0.1712
    0.6787    0.7060
    0.7577    0.0318

h =
    0.2769    0.8235
    0.0462    0.6948
    0.0971    0.3171

newh =  ReshapeColumnProduct(eh, h, 0);

    0.2867    1.2913
    0.1266    0.4520

newh = ReshapeColumnProduct(eh, h, 1);

    0.2657    1.0923
    0.3636    0.6416

===================================================
W1 =
    0.3404    0.2238    0.2551
    0.5853    0.7513    0.5060

a1 =
    0.1386
    0.1493
    0.2575

W2 =
    0.6991    0.9593
    0.8909    0.5472

a2 =
    0.8407
    0.2543

v =

    0.1869    0.4456    0.7094    0.2760    0.6551
    0.4898    0.6463    0.7547    0.6797    0.1626

[h,h1,h2]=dtnnforward(W1, a1, W2, a2, v)

h =
    0.4980    0.5614    0.6125    0.5465    0.4953
    0.5112    0.5739    0.6223    0.5636    0.4878
    0.5100    0.5671    0.6122    0.5543    0.5049
    0.4144    0.4879    0.5514    0.4627    0.4442
    0.4253    0.4987    0.5602    0.4772    0.4375
    0.4244    0.4928    0.5511    0.4694    0.4527
    
h1 =
    0.6198    0.6612    0.6946    0.6526    0.6123
    0.6362    0.6758    0.7058    0.6730    0.6030
    0.6348    0.6678    0.6943    0.6619    0.6241
    
h2 =
    0.8034    0.8492    0.8817    0.8374    0.8090
    0.6685    0.7380    0.7938    0.7091    0.7255

============================================================

further set dW1, da1, dW2, da2 to 0s,

eh =
    0.8143    0.6160    0.9172    0.0759    0.5688
    0.2435    0.4733    0.2858    0.0540    0.4694
    0.9293    0.3517    0.7572    0.5308    0.0119
    0.3500    0.8308    0.7537    0.7792    0.3371
    0.1966    0.5853    0.3804    0.9340    0.1622
    0.2511    0.5497    0.5678    0.1299    0.7943

learning rate per sample = 0.1, momentum = 0.9

[W1, a1, W2, a2, dW1, da1, dW2, da2, ev] = dtnnbackward(W1, a1, W2, a2, dW1, da1, dW2, da2, v, h1, h2, eh, 0.1, 0.9)

W1 =
    0.3455    0.2268    0.2591
    0.5914    0.7549    0.5108

a1 =
    0.1493
    0.1558
    0.2661

W2 =
    0.7018    0.9637
    0.8943    0.5528

a2 =
    0.8469
    0.2640

dW1 =
    0.0512    0.0298    0.0401
    0.0614    0.0367    0.0488

da1 =
    0.1069
    0.0648
    0.0863

dW2 =
    0.0274    0.0446
    0.0340    0.0562

da2 =
    0.0621
    0.0971

ev =
    0.3868    0.4969    0.4737    0.3966    0.3405
    0.5233    0.6139    0.6142    0.4505    0.4320
*/