#pragma once

#include "RenderObject.h"
#include "Lib\poly2tri\poly2tri.h"

#include <vector>

namespace NetEditor
{
	class Edit
	{
	public:
		enum eOperation
		{
			Add,
			Remove,
		};

		enum eType
		{
			Contour,
			Hole,
		};

		std::vector<Radar::RenderObject *> renders;

		void setOp(eOperation op) { this->_op = op; }
		void freePoint() { delete _pt; }

		eType type() { return this->_type; }
		p2t::Point *point() { return this->_pt; }

		Edit(eOperation op, eType type, p2t::Point *pt)
		{
			this->_type = type;
			this->_pt = pt;
			this->_op = op;
		}

		Edit() {};
		~Edit();

	private:
		eType _type;
		eOperation _op;
		p2t::Point *_pt;

	};
}
