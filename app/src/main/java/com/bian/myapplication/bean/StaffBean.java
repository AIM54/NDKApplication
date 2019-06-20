package com.bian.myapplication.bean;

public class StaffBean {
    private String name;
    private int age;
    private double inCome;
    private String address;

    public StaffBean(String name, int age, double inCome, String address) {
        this.name = name;
        this.age = age;
        this.inCome = inCome;
        this.address = address;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }

    public double getInCome() {
        return inCome;
    }

    public void setInCome(double inCome) {
        this.inCome = inCome;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }
}
