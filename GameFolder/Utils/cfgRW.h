#pragma once

#include "../Component.hpp"
#include "../SceneABS.h"
#include <string>

namespace CFG
{

	inline const std::string formatComponent(CTransform& cT)
	{
		std::string cTransform;

		cTransform += "CTransform ";
		cTransform += std::to_string(cT.pos.x) + " " + std::to_string(cT.pos.y) + " ";
		cTransform += std::to_string(cT.velocity.x) + " " + std::to_string(cT.velocity.y) + " ";
		cTransform += std::to_string(cT.angle);
		cTransform += "\n";

		return cTransform;
	}

	inline const std::string formatComponent(CBoundingBox& cBB)
	{
		std::string cBoundingBox;

		cBoundingBox += "CBoundingBox ";
		cBoundingBox += std::to_string(cBB.size.x) + " " + std::to_string(cBB.size.y);
		cBoundingBox += "\n";

		return cBoundingBox;
	}

	inline const std::string formatComponent(CSprite& cS, std::string textureName)
	{
		std::string cSprite;

		cSprite += "CSprite ";
		cSprite += textureName + " ";
		cSprite += std::to_string(cS.sprite.getTextureRect().left) + " ";
		cSprite += std::to_string(cS.sprite.getTextureRect().top) + " ";
		cSprite += std::to_string(cS.sprite.getTextureRect().width) + " ";
		cSprite += std::to_string(cS.sprite.getTextureRect().height) + "\n";

		return cSprite;
	}
}