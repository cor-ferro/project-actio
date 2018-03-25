#ifndef RENDERER_TYPES_H_
#define RENDERER_TYPES_H_


namespace renderer {
	typedef unsigned int ScreenSize;

	struct Params {
		renderer::ScreenSize width;
		renderer::ScreenSize height;
		float aspectRatio;

		void calcAspectRatio() {
			aspectRatio = (float)width / (float)height;
		}
	};
}


#endif /* RENDERER_TYPES_H_ */
