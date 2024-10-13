#include <gtkmm.h>
#include <iostream>

class CandidateEditor : public Gtk::Box {
public:
    CandidateEditor();

    Gtk::Entry *w_candidate_name = nullptr;
    Gtk::SpinButton *w_votes = nullptr;
    Gtk::Button *w_del = nullptr;
};

class ControlWindow : public Gtk::Window {
public:
    ControlWindow();
    void update_view();
    void add_candidate();

private:
    Gtk::ListBox *lb = nullptr;
    class ViewWindow *view_window = nullptr;
    Gtk::Entry *w_title = nullptr;
    Gtk::SpinButton *w_total_votes = nullptr;
    Gtk::SpinButton *w_text_size = nullptr;
};

class ViewWindow : public Gtk::Window {
public:
    ViewWindow();
    Gtk::ListBox *lb = nullptr;
    Gtk::Label *w_title = nullptr;
    Gtk::Label *w_total = nullptr;
    Gtk::Image *w_icon = nullptr;
};

class ViewRow : public Gtk::Box {
public:
    ViewRow(const std::string &la, int n, float fill, double scale, bool out);
};

CandidateEditor::CandidateEditor() : Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 8)
{
    property_margin() = 4;
    w_candidate_name = Gtk::manage(new Gtk::Entry);
    pack_start(*w_candidate_name, true, true, 0);
    w_candidate_name->show();

    w_votes = Gtk::manage(new Gtk::SpinButton);
    w_votes->set_range(0, 1000);
    w_votes->set_increments(1, 1);
    w_votes->set_digits(0);
    pack_start(*w_votes, false, false, 0);
    w_votes->show();

    {
        w_del = Gtk::manage(new Gtk::Button);
        w_del->set_margin_start(4);
        w_del->set_image_from_icon_name("list-remove-symbolic", Gtk::ICON_SIZE_BUTTON);
        pack_start(*w_del, false, false, 0);
        w_del->show();
    }
}

ControlWindow::ControlWindow() : Gtk::Window()
{
    set_title("Abstimmimimi");
    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0));

    auto tbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));

    auto add_button = Gtk::manage(new Gtk::Button("Neue Option"));
    add_button->signal_clicked().connect([this] { add_candidate(); });
    tbox->pack_start(*add_button, false, false, 0);
    {
        auto la = Gtk::manage(new Gtk::Label("Abstimmung"));
        la->set_margin_start(5);
        la->get_style_context()->add_class("dim-label");
        tbox->pack_start(*la, false, false, 0);
    }
    w_title = Gtk::manage(new Gtk::Entry);
    w_title->signal_changed().connect([this] { update_view(); });

    tbox->pack_start(*w_title, true, true, 0);

    {
        auto la = Gtk::manage(new Gtk::Label("Stimmen"));
        la->set_margin_start(5);
        la->get_style_context()->add_class("dim-label");
        tbox->pack_start(*la, false, false, 0);
    }

    w_total_votes = Gtk::manage(new Gtk::SpinButton());
    w_total_votes->set_range(1, 1000);
    w_total_votes->set_increments(1, 10);
    w_total_votes->signal_value_changed().connect([this] { update_view(); });

    tbox->pack_start(*w_total_votes, false, false, 0);

    {
        auto la = Gtk::manage(new Gtk::Label("Schrift"));
        la->set_margin_start(5);
        la->get_style_context()->add_class("dim-label");
        tbox->pack_start(*la, false, false, 0);
    }

    w_text_size = Gtk::manage(new Gtk::SpinButton());
    w_text_size->set_range(1, 10);
    w_text_size->set_value(5);
    w_text_size->set_increments(1, 1);
    w_text_size->signal_value_changed().connect([this] { update_view(); });

    tbox->pack_start(*w_text_size, false, false, 0);

    box->pack_start(*tbox, false, false, 0);

    tbox->property_margin() = 5;
    {
        auto sep = Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_HORIZONTAL));
        box->pack_start(*sep, false, false, 0);
    }

    lb = Gtk::manage(new Gtk::ListBox);
    lb->set_selection_mode(Gtk::SELECTION_NONE);

    box->pack_start(*lb, true, true, 0);
    add(*box);
    box->show_all();

    view_window = new ViewWindow();

    view_window->present();
}

void ControlWindow::add_candidate()
{
    auto e = Gtk::manage(new CandidateEditor);
    e->w_candidate_name->signal_changed().connect([this] { update_view(); });
    e->w_votes->signal_value_changed().connect([this] { update_view(); });
    e->w_del->signal_clicked().connect([this, e] {
        delete e->get_parent();
        update_view();
    });
    e->w_candidate_name->signal_activate().connect([this, e] {
        auto row = dynamic_cast<Gtk::ListBoxRow *>(e->get_ancestor(GTK_TYPE_LIST_BOX_ROW));
        auto index = row->get_index();
        if (index >= 0) {
            if (auto nextrow = lb->get_row_at_index(index + 1)) {
                auto e_next = dynamic_cast<CandidateEditor *>(nextrow->get_child());
                e_next->w_candidate_name->grab_focus();
            }
            else {
                add_candidate();
            }
        }
    });
    lb->append(*e);
    e->show();
    e->w_candidate_name->grab_focus();
    update_view();
}

struct Vote {
    std::string name;
    unsigned int count;
    bool valid;
};

void ControlWindow::update_view()
{
    std::vector<Vote> votes;

    int votes_total_expected = w_total_votes->get_value_as_int();

    int votes_total = 0;
    int votes_excluded = 0;
    int votes_remaining = votes_total_expected;
    {
        auto children = lb->get_children();
        for (auto ch : children) {
            auto row = dynamic_cast<Gtk::ListBoxRow *>(ch);
            auto ed = dynamic_cast<CandidateEditor *>(row->get_child());
            unsigned int n = ed->w_votes->get_value_as_int();
            std::string name = ed->w_candidate_name->get_text();
            bool valid = true;
            votes_remaining -= n;
            if (name.size() && name.at(0) == '#') {
                votes_excluded += n;
                name = "(" + name.substr(1) + ")";
                valid = false;
            }
            else {
                votes_total += n;
                valid = true;
            }
            votes.push_back({name, n, valid});
        }
    }

    view_window->w_title->set_text(w_title->get_text());
    std::string votes_str = std::to_string(votes_total);
    if (votes_excluded > 0) {
        votes_str += "+" + std::to_string(votes_excluded);
    }
    votes_str += " / " + std::to_string(votes_total_expected);
    view_window->w_total->set_text(votes_str);
    const auto real_total = votes_total + votes_excluded;
    for (auto ctx : {view_window->w_total->get_style_context(), view_window->w_icon->get_style_context()}) {
        ctx->remove_class("votes-invalid");
        ctx->remove_class("votes-complete");
        ctx->remove_class("no-backdrop");

        if (real_total == votes_total_expected)
            ctx->add_class("votes-complete");
        else if (real_total > votes_total_expected)
            ctx->add_class("votes-invalid");
        else
            ctx->add_class("no-backdrop");
    }
    view_window->w_icon->set_visible(true);
    if (real_total == votes_total_expected) {
        view_window->w_icon->set_from_icon_name("object-select-symbolic", Gtk::ICON_SIZE_DIALOG);
    }
    else if (real_total > votes_total_expected) {
        view_window->w_icon->set_from_icon_name("dialog-warning-symbolic", Gtk::ICON_SIZE_DIALOG);
    }
    else {
        view_window->w_icon->set_visible(false);
    }

    {
        auto children = view_window->lb->get_children();
        for (auto ch : children) {
            delete ch;
        }
    }

    std::sort(votes.begin(), votes.end(), [](const auto &a, const auto &b) { return a.count > b.count; });

    bool all_out = true;
    for (const auto &it : votes) {
        std::string label = it.name;
        if (it.valid && (it.count > ((votes_total_expected - votes_excluded) / 2))) {
            label += " (Abs. Mehrheit)";
        }
        const bool out = (2 * (it.count + votes_remaining) < (votes_total_expected - votes_excluded));
        if (it.valid)
            all_out = all_out && out;
        auto r = Gtk::make_managed<ViewRow>(label, it.count,
                                            static_cast<float>(it.count) / (votes_total_expected - votes_excluded),
                                            w_text_size->get_value(), out && it.valid);
        view_window->lb->append(*r);
        r->show();
    }
    {
        Pango::AttrList alist;
        auto attr_scale = Pango::Attribute::create_attr_scale(5);
        alist.insert(attr_scale);
        auto strike = Pango::Attribute::create_attr_strikethrough(true);
        if (all_out)
            alist.insert(strike);
        view_window->w_title->set_attributes(alist);
    }
}

class MyProgressbar : public Gtk::DrawingArea {
public:
    MyProgressbar();
    void set_level(float l);

protected:
    float level = 0;
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
};

MyProgressbar::MyProgressbar()
{
    set_size_request(-1, 20);
}

bool MyProgressbar::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    cr->set_source_rgb(.9, .9, .9);
    cr->paint();
    auto width = get_allocated_width();
    cr->set_source_rgb(1, 90 / 255., 23 / 255.);
    cr->rectangle(0, 0, width * level, get_allocated_height());
    cr->fill();
    cr->set_source_rgb(0, 0, 0);
    cr->set_line_width(2);
    cr->move_to(width / 2, 0);
    cr->rel_line_to(0, get_allocated_height());
    cr->stroke();
    return true;
}

void MyProgressbar::set_level(float l)
{
    level = l;
    queue_draw();
}

ViewRow::ViewRow(const std::string &la, int n, float fill, double scale, bool out)
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0)
{

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    Pango::AttrList alist;
    Pango::AttrList alist_label;
    auto attr_scale = Pango::Attribute::create_attr_scale(scale);
    auto strike = Pango::Attribute::create_attr_strikethrough(true);
    if (out)
        alist_label.insert(strike);
    alist.insert(attr_scale);
    alist_label.insert(attr_scale);

    {
        std::string label_str = la;
        auto label = Gtk::manage(new Gtk::Label(label_str));
        label->set_xalign(0);
        label->set_attributes(alist_label);
        label->set_margin_start(5);
        label->set_ellipsize(Pango::ELLIPSIZE_END);
        label->get_style_context()->add_class("no-backdrop");
        box->pack_start(*label, false, false, 0);
    }

    {
        auto label = Gtk::manage(new Gtk::Label(std::to_string(n)));
        label->get_style_context()->add_class("dim-label");
        label->set_xalign(1);
        label->set_attributes(alist);
        label->set_margin_end(5);
        label->get_style_context()->add_class("no-backdrop");
        box->pack_start(*label, true, true, 0);
    }

    pack_start(*box, false, false, 0);
    box->show_all();

    auto pr = Gtk::manage(new MyProgressbar);
    pr->set_size_request(-1, 4 * scale);
    pr->set_level(fill);
    pack_start(*pr, false, false, 0);
    pr->show();

    set_margin_top(5);
    set_margin_bottom(5);
}

ViewWindow::ViewWindow() : Gtk::Window()
{
    set_title("Abstimmimimi");
    get_style_context()->add_class("ab-view-win");
    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

    auto box2 = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));

    Pango::AttrList alist;
    auto attr_scale = Pango::Attribute::create_attr_scale(5);
    alist.insert(attr_scale);

    w_title = Gtk::manage(new Gtk::Label("Probeabstimmung"));
    w_title->get_style_context()->add_class("no-backdrop");
    w_title->property_margin() = 10;
    w_title->set_xalign(0);
    w_title->set_attributes(alist);
    w_title->set_ellipsize(Pango::ELLIPSIZE_END);

    w_total = Gtk::manage(new Gtk::Label("0"));
    w_total->get_style_context()->add_class("dim-label");
    w_total->property_margin() = 10;
    w_total->set_xalign(1);
    w_total->set_attributes(alist);

    w_icon = Gtk::manage(new Gtk::Image);
    w_icon->set_margin_end(20);
    w_icon->set_from_icon_name("dialog-warning-symbolic", Gtk::ICON_SIZE_DIALOG);

    box2->pack_start(*w_title, true, true, 0);
    box2->pack_start(*w_icon, false, false, 0);
    box2->pack_start(*w_total, false, false, 0);
    box->pack_start(*box2, false, false, 0);

    {
        auto sep = Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_HORIZONTAL));
        box->pack_start(*sep, false, false, 0);
    }

    lb = Gtk::manage(new Gtk::ListBox);
    lb->set_selection_mode(Gtk::SELECTION_NONE);

    box->pack_start(*lb, true, true, 0);

    add(*box);
    box->show_all();

    signal_delete_event().connect([this](GdkEventAny *ev) { return true; });
}

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create(argc, argv, "de.selfnet.abstimmimimi");

    auto cssp = Gtk::CssProvider::create();
    cssp->load_from_data(
            "* {font-feature-settings: \"tnum 1\";} "
            ".votes-complete {color: rgb(0,200,0);} "
            ".votes-invalid  {color: rgb(255,0,0);} "
            ".no-backdrop {color: @theme_fg_color;} "
            /*".votes-complete:backdrop {color: rgb(0,200,0);} "
            ".votes-invalid:backdrop  {color: rgb(255,0,0);} "
            ".ab-view-win label:backdrop{color:@theme_fg_color;} "*/
    );
    Gtk::StyleContext::add_provider_for_screen(Gdk::Screen::get_default(), cssp,
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    ControlWindow window;

    window.set_default_size(200, 200);

    return app->run(window);
}
