#pragma once
#include <memory>
#include "Entity.h"
#include "Vec2.h"
#include <math.h>

namespace Physics
{
	Vec2 getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
	{
		auto cTa = a->getComponent<CTransform>();
		auto cTb = b->getComponent<CTransform>();

		auto cBa = a->getComponent<CBoundingBox>();
		auto cBb = b->getComponent<CBoundingBox>();

		Vec2 delta(	abs((cTa.pos.x + cBa.offset.x) - (cTb.pos.x + cBb.offset.x)),
					abs((cTa.pos.y + cBa.offset.y) - (cTb.pos.y + cBb.offset.y)));

		Vec2 overlap;
		overlap.x = (cBa.halfSize.x + cBb.halfSize.x) - delta.x;
		overlap.y = (cBa.halfSize.y + cBb.halfSize.y) - delta.y;

		return overlap;
	}

	Vec2 getPrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
	{
		auto cTa = a->getComponent<CTransform>();
		auto cTb = b->getComponent<CTransform>();

		auto cBa = a->getComponent<CBoundingBox>();
		auto cBb = b->getComponent<CBoundingBox>();

		Vec2 delta(	abs((cTa.prevPos.x + cBa.offset.x) - (cTb.prevPos.x + cBb.offset.x)),
					abs((cTa.prevPos.y + cBa.offset.y) - (cTb.prevPos.y + cBb.offset.y)));

		Vec2 overlap;
		overlap.x = (cBa.halfSize.x + cBb.halfSize.x) - delta.x;
		overlap.y = (cBa.halfSize.y + cBb.halfSize.y) - delta.y;

		return overlap;
	}
}