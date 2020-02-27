#include "TweenDescr.h"
#include "Maths.h"

bool TweenDescr::init()
{
	return true;
}

bool TweenDescr::updateInternal()
{
	float directionLocal = direction;
	/*if (this.usesNormalDt) {
		dt = LeanTween.dtActual;
	}
	else if (this.useEstimatedTime) {
		dt = LeanTween.dtEstimated;
	}
	else if (this.useFrames) {
		dt = this.optional.initFrameCount == 0 ? 0 : 1;
		this.optional.initFrameCount = Time.frameCount;
	}
	else if (this.useManualTime) {
		dt = LeanTween.dtManual;
	}*/

	//		Debug.Log ("tween:" + this+ " dt:"+dt);
	if (delay <= 0.0f && directionLocal != 0.0f) {
		if (trans == nullptr)
		{
			LOG_ENGINE("There's no trans component in Tween!");
			return true;
		}

		// initialize if has not done so yet
		if (!hasInitiliazed)
			init();

		dt = dt * directionLocal;
		passed += dt;

		passed = Maths::Clamp(passed, 0.0f, time);

		ratioPassed = (passed / time); // need to clamp when finished so it will finish at the exact spot and not overshoot

		/*easeInternal();*/

		/*if (this.hasUpdateCallback)
			this._optional.callOnUpdate(val, this.ratioPassed);*/

		bool isTweenFinished = directionLocal > 0.0f ? passed >= time : passed <= 0f;
		//			Debug.Log("lt "+this+" dt:"+dt+" fin:"+isTweenFinished);
		if (isTweenFinished) { // increment or flip tween
			this.loopCount--;
			if (this.loopType == UITweenType.pingPong) {
				this.direction = 0.0f - directionLocal;
			}
			else {
				this.passed = Mathf.Epsilon;
			}

			isTweenFinished = this.loopCount == 0 || this.loopType == LeanTweenType.once; // only return true if it is fully complete

			if (isTweenFinished == false && this.onCompleteOnRepeat && this.hasExtraOnCompletes)
				callOnCompletes(); // this only gets called if onCompleteOnRepeat is set to true, otherwise LeanTween class takes care of calling it

			return isTweenFinished;
		}
	}
	else {
		delay -= dt;
	}

	return false;
}

void TweenDescr::callOnCompletes()
{
	if (type == UITween.GUI_ROTATE)
		this._optional.ltRect.rotateFinished = true;

	if (this.type == TweenAction.DELAYED_SOUND) {
		AudioSource.PlayClipAtPoint((AudioClip)this._optional.onCompleteParam, this.to, this.from.x);
	}
	if (this._optional.onComplete != null) {
		this._optional.onComplete();
	}
	else if (this._optional.onCompleteObject != null) {
		this._optional.onCompleteObject(this._optional.onCompleteParam);
	}
}

bool TweenDescr::easeInternal()
{
	return true;
}
