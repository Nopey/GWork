/*
 *  Gwork
 *  Copyright (c) 2010 Facepunch Studios
 *  Copyright (c) 2013-16 Billy Quith
 *  See license in Gwork.h
 */

#include <Gwork/Test/Test.h>
#include <Gwork/Controls/ComboBox.h>

using namespace Gwk;

class ComboBox : public GUnit
{
public:

    GWK_CONTROL_INLINE(ComboBox, GUnit)
    {
        {
            Gwk::Controls::ComboBox* combo = new Gwk::Controls::ComboBox(this);
            combo->SetPos(50, 50);
            combo->SetWidth(200);
            combo->AddItem("Option One", "one");
            combo->AddItem("Number Two", "two");
            combo->AddItem("Door Three", "three");
            combo->AddItem("Four Legs", "four");
            combo->AddItem("Five Birds", "five");
            combo->onSelection.Add(this, &ComboBox::OnComboSelect);
        }
        {
            // Empty..
            Gwk::Controls::ComboBox* combo = new Gwk::Controls::ComboBox(this);
            combo->SetPos(50, 80);
            combo->SetWidth(200);
        }
        {
            // Empty..
            Gwk::Controls::ComboBox* combo = new Gwk::Controls::ComboBox(this);
            combo->SetPos(50, 110);
            combo->SetWidth(200);

            for (int i = 0; i < 500; i++)
            {
                combo->AddItem("Lots Of Options");
            }
        }
    }

    void OnComboSelect(Gwk::Controls::Base* pControl)
    {
        Gwk::Controls::ComboBox* combo = (Gwk::Controls::ComboBox*)pControl;
        UnitPrint(Utility::Format("Combo Changed: %s",
                                  combo->GetSelectedItem()->GetText().c_str()));
    }

};


DEFINE_UNIT_TEST(ComboBox);