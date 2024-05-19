#pragma once
#include <memory>
#include "Entity.h"
#include "Vec2.h"
#include <math.h>

namespace Physics
{
	Vec2 getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
	{
		Vec2 delta(	abs(a->getComponent<CTransform>().pos.x - b->getComponent<CTransform>().pos.x),
					abs(a->getComponent<CTransform>().pos.y - b->getComponent<CTransform>().pos.y)	);

		Vec2 overlap;
		overlap.x = (a->getComponent<CBoundingBox>().halfSize.x + b->getComponent<CBoundingBox>().halfSize.x) - delta.x;
		overlap.y = (a->getComponent<CBoundingBox>().halfSize.y + b->getComponent<CBoundingBox>().halfSize.y) - delta.y;

		return overlap;
	}

	Vec2 getPrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
	{
		Vec2 delta(	abs(a->getComponent<CTransform>().prevPos.x - b->getComponent<CTransform>().prevPos.x),
					abs(a->getComponent<CTransform>().prevPos.y - b->getComponent<CTransform>().prevPos.y));

		Vec2 overlap;
		overlap.x = (a->getComponent<CBoundingBox>().halfSize.x + b->getComponent<CBoundingBox>().halfSize.x) - delta.x;
		overlap.y = (a->getComponent<CBoundingBox>().halfSize.y + b->getComponent<CBoundingBox>().halfSize.y) - delta.y;

		return overlap;
	}
}