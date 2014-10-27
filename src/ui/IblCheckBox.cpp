//------------------------------------------------------------------------------------//
//                                                                                    //
//    ._____________.____   __________         __                                     //
//    |   \______   \    |  \______   \_____  |  | __ ___________                     //
//    |   ||    |  _/    |   |    |  _/\__  \ |  |/ // __ \_  __ \                    //
//    |   ||    |   \    |___|    |   \ / __ \|    <\  ___/|  | \/                    //
//    |___||______  /_______ \______  /(____  /__|_ \\___  >__|                       //
//                \/        \/      \/      \/     \/    \/                           //
//                                                                                    //
//    IBLBaker is provided under the MIT License(MIT)                                 //
//    IBLBaker uses portions of other open source software.                           //
//    Please review the LICENSE file for further details.                             //
//                                                                                    //
//    Copyright(c) 2014 Matt Davidson                                                 //
//                                                                                    //
//    Permission is hereby granted, free of charge, to any person obtaining a copy    //
//    of this software and associated documentation files(the "Software"), to deal    //
//    in the Software without restriction, including without limitation the rights    //
//    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell     //
//    copies of the Software, and to permit persons to whom the Software is           //
//    furnished to do so, subject to the following conditions :                       //
//                                                                                    //
//    1. Redistributions of source code must retain the above copyright notice,       //
//    this list of conditions and the following disclaimer.                           //
//    2. Redistributions in binary form must reproduce the above copyright notice,    //
//    this list of conditions and the following disclaimer in the                     //
//    documentation and / or other materials provided with the distribution.          //
//    3. Neither the name of the copyright holder nor the names of its                //
//    contributors may be used to endorse or promote products derived                 //
//    from this software without specific prior written permission.                   //
//                                                                                    //
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      //
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        //
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE      //
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          //
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
//    THE SOFTWARE.                                                                   //
//                                                                                    //
//------------------------------------------------------------------------------------//

#include <IblCheckBox.h>
#include <IblWidgetDialog.h>

namespace Ibl
{
CheckBox::CheckBox (Dialog *dialog, Ibl::IDevice* renderMgr ) : 
    Button (dialog, renderMgr)
{
    _type = _CONTROL_CHECKBOX;

    m_bChecked = false;
}
    
bool CheckBox::handleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !_enabled || !visible() )
        return false;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_SPACE:
                    _pressed = true;
                    return true;
            }
        }

        case WM_KEYUP:
        {
            switch( wParam )
            {
                case VK_SPACE:
                    if( _pressed == true )
                    {
                        _pressed = false;
                        SetCheckedInternal( !m_bChecked, true );
                    }
                    return true;
            }
        }
    }
    return false;
}

bool CheckBox::handleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !_enabled || !visible() )
        return false;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if (containsPoint( pt ))
            {
                // Pressed while inside the control
                _pressed = true;
                // mdavidson come back to
                SetCapture (_dialog->windowHandle());

                if( !_hasFocus )
                    _dialog->RequestFocus (this);

                return true;
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if( _pressed )
            {
                _pressed = false;
                ReleaseCapture();

                // Button click
                if( containsPoint( pt ) )
                {
                    SetCheckedInternal( !m_bChecked, true );

                }

                return true;
            }

            break;
        }
    };
    
    return false;
}

void CheckBox::SetCheckedInternal( bool bChecked, bool bFromInput ) 
{ 
    m_bChecked = bChecked; 

    _dialog->SendEvent( EVENT_CHECKBOX_CHANGED, bFromInput, this ); 
}

BOOL CheckBox::containsPoint( POINT pt ) 
{ 
    //MATT, check containspoint code base

   BOOL Result = ( PtInRect( &_boundingBox, pt ) || 
             PtInRect( &_buttonBounds, pt ) ); 

    return Result;
}

void CheckBox::updateRects()
{
    Button::updateRects();

    _buttonBounds = _boundingBox;
    _buttonBounds.right = _buttonBounds.left + RectHeight( _buttonBounds );

    m_rcText = _boundingBox;
    m_rcText.left += (int) ( 1.25f * RectWidth( _buttonBounds ) );
}

void CheckBox::render( float elapsedTime )
{
    _CONTROL_STATE iState = _STATE_NORMAL;

    if( visible() == false )
        iState = _STATE_HIDDEN;
    else if( _enabled == false )
        iState = _STATE_DISABLED;
    else if( _pressed )
        iState = _STATE_PRESSED;
    else if( _mouseOver )
        iState = _STATE_MOUSEOVER;
    else if( _hasFocus )
        iState = _STATE_FOCUS;

    
    UIElement* pElement = _elements[0];

    float fBlendRate = ( iState == _STATE_PRESSED ) ? 0.0f : 0.8f;

    pElement->TextureColor.Blend( iState, elapsedTime, fBlendRate );
    pElement->FontColor.Blend( iState, elapsedTime, fBlendRate );

    _dialog->DrawSprite( pElement, &_buttonBounds );
    _dialog->DrawText( m_strText, pElement, &m_rcText, true );

    if( !m_bChecked )
    {
        iState = _STATE_HIDDEN;
    }
    else
    {
        pElement = _elements[1];

        pElement->TextureColor.Blend( iState, elapsedTime, fBlendRate );
       _dialog->DrawSprite( pElement, &_buttonBounds );
    }
}

void CheckBox::hotkey()
{ 
    _dialog->RequestFocus( this ); SetCheckedInternal( !m_bChecked, true ); 
}

}