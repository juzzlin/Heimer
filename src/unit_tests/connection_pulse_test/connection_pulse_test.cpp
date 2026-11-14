// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
//
// Heimer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Heimer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Heimer. If not, see <http://www.gnu.org/licenses/>.

#include "connection_pulse_test.hpp"

#include "../../view/scene_items/connection_pulse.hpp"

#include <QImage>
#include <QPainter>
#include <QTest>

using SceneItems::ConnectionPulse;

namespace {

QImage render(ConnectionPulse & pulse, qreal progress)
{
    pulse.setProgress(progress);
    const auto rect = pulse.boundingRect();
    QImage image { rect.size().toSize(), QImage::Format_ARGB32_Premultiplied };
    image.fill(Qt::transparent);
    QPainter painter { &image };
    painter.translate(-rect.topLeft());
    pulse.paint(&painter, nullptr);
    return image;
}

bool hasVisiblePixels(const QImage & image)
{
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            if (image.pixelColor(x, y).alpha() > 8) {
                return true;
            }
        }
    }
    return false;
}

} // namespace

void ConnectionPulseTest::test_start_shouldShowAndFinallyHide()
{
    ConnectionPulse pulse;
    QVERIFY(!pulse.isVisible());

    pulse.start();

    QVERIFY(pulse.isVisible());
    QCOMPARE(pulse.progress(), 0.0);
    QTRY_VERIFY_WITH_TIMEOUT(!pulse.isVisible(), 5000);
    QCOMPARE(pulse.progress(), 1.0);
}

void ConnectionPulseTest::test_stop_shouldHide()
{
    ConnectionPulse pulse;
    pulse.start();

    pulse.stop();

    QVERIFY(!pulse.isVisible());
}

void ConnectionPulseTest::test_paint_shouldDrawRingsInColorAndNothingWhenFinished()
{
    ConnectionPulse pulse;
    pulse.setColor({ 255, 0, 0, 10 }); // The alpha is ignored

    const auto image = render(pulse, 0.3);
    const auto center = image.rect().center();
    const auto dot = image.pixelColor(center);
    QCOMPARE(dot, QColor(255, 0, 0));
    // Somewhere on the expanding ring
    bool ringFound = false;
    for (int x = center.x() + 8; x < image.width(); x++) {
        if (const auto color = image.pixelColor(x, center.y()); color.alpha() > 64 && color.red() > 200 && color.green() < 64) {
            ringFound = true;
        }
    }
    QVERIFY(ringFound);

    QVERIFY(!hasVisiblePixels(render(pulse, 1.0)));
}

QTEST_MAIN(ConnectionPulseTest)
