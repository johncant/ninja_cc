#ifndef __VISUALIZER_H__
#define __VISUALIZER_H__

// Just so you can use fake static polymorphism here

class VisualizerImpl;

class VisualWrapperBase {
  friend class Visualizer;
  protected:
  virtual void draw()=0;
  virtual void configure()=0;
  virtual void initialize()=0;
  virtual ~VisualWrapperBase() {};
};

// Just so you can use fake static polymorphism here
template <class visual_t>
class VisualWrapper : public VisualWrapperBase {
  visual_t& visual;
  friend class Visualizer;
  VisualWrapper(visual_t& v) : visual(v) { }
  ~VisualWrapper() {}
  // Delegate everything
  void draw() { visual.draw(); }
  void configure() { visual.configure(); }
  void initialize() { visual.initialize(); }
};


// Fake singleton - only make one of these please!
class Visualizer {
  VisualizerImpl* mkimpl();
  public:

  template <class visual_t>
  Visualizer(visual_t& v) : visual(new VisualWrapper<visual_t>(v)), pimpl(mkimpl()) {}
  ~Visualizer();
  void mainloop(); // Displays window
  void initialize();
  void configure(); // Need dimensions
  void draw();
  template<class visual_t>
  void change_visual(visual_t& v) {
    delete visual;
    visual = new VisualWrapper<visual_t>(v);
  }

  void destroy();

  private:
  VisualWrapperBase* visual;
  VisualizerImpl* pimpl;
};

#endif
